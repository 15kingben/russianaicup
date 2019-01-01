import numpy as np
from time import time
import tensorflow as tf
from Logger import Logger
from multiprocessing import Process, Queue
from CompetitiveSnake import CompetitiveSnake

CLIP = 0.2
EPOCHS = 10
GAMMA = 0.99
LAMBDA = 0.95
HORIZON = 2048
NUM_HIDDEN = 2
BATCH_SIZE = 64
HIDDEN_SIZE = 128
QUEUE_SIZE = 100
NUM_WORKERS = 10
GUI = False
if GUI and NUM_WORKERS > 2:
    print("Uh oh")
    exit()
RESTORE_INTERVAL = 5 #seconds
NUM_ITERATIONS = int(1e9)
MODEL_PATH = "models/policycb"

MIN_LOGSTD = np.log(0.5)
MAX_LOGSTD = np.log(1.5)
NUM_LOGSTD_STEPS = 3000

def initializer(std):
    def _initializer(shape, dtype=None, partition_info=None):
        out = np.random.randn(*shape).astype(np.float32)
        out *= std / np.sqrt(np.square(out).sum(axis=0, keepdims=True))
        return tf.constant(out)
    return _initializer

class GaussianDist(object):
    def __init__(self, flat):
        mean, logstd = tf.split(
            axis=len(flat.shape)-1,
            num_or_size_splits=2,
            value=flat
        )
        self.mean = mean
        self.logstd = logstd
        self.std = tf.exp(self.logstd)

    def logp(self, x):
        logprob = -0.5
        logprob *= tf.square((x - self.mean) / self.std)
        logprob = tf.reduce_sum(logprob, axis=-1)
        logprob -= 0.5 * np.log(2.0 * np.pi) * tf.to_float(tf.shape(x)[-1])
        logprob -= tf.reduce_sum(self.logstd, axis=-1)
        return logprob

    def entropy(self):
        return tf.reduce_sum(self.logstd + .5 * np.log(2.0 * np.pi * np.e), axis=-1)

    def sample(self):
        return self.mean + self.std * tf.random_normal(tf.shape(self.mean))

    def mode(self):
        return self.mean


class ContinuousPolicy(object):

    def __init__(self, obv_, action_, num_hidden, hidden_size, scope=""):
        with tf.variable_scope(scope):
            pol_hidden = obv_
            val_hidden = obv_
            for i in range(num_hidden):
                pol_hidden = tf.layers.dense(
                    pol_hidden,
                    hidden_size,
                    name="pol_hidden{}".format(i+1),
                    kernel_initializer=initializer(1.0)
                )
                val_hidden = tf.layers.dense(
                    val_hidden,
                    hidden_size,
                    name="val_hidden{}".format(i+1),
                    kernel_initializer=initializer(1.0)
                )
                pol_hidden = tf.nn.tanh(pol_hidden)
                val_hidden = tf.nn.tanh(val_hidden)

            mean = tf.layers.dense(
                pol_hidden,
                action_.shape[1],
                name="mean",
                kernel_initializer=initializer(0.01)
            )
            value = tf.layers.dense(
                val_hidden,
                action_.shape[1],
                name="value",
                kernel_initializer=initializer(1.0)
            )
            self.logstd = tf.placeholder(tf.float32)
            param = tf.concat([mean, mean * 0.0 + self.logstd], axis=1)
            self.obv_ = obv_
            self.value = value[:, 0]
            self.dist = GaussianDist(param)
            self.stochastic_action = self.dist.sample()
            self.deterministic_action = self.dist.mode()
            self.variables = tf.get_collection(tf.GraphKeys.GLOBAL_VARIABLES, scope)

class Dataset(object):

    def __init__(self, seg, keys):
        self.seg = seg
        self.keys = keys
        self.length = len(seg[keys[0]])

    def iterate(self, batch_size):
        indices = np.arange(self.length)
        np.random.shuffle(indices)
        for key in self.keys:
            self.seg[key] = self.seg[key][indices]
        for i in range(int(self.length / batch_size)):
            begin =   i   * batch_size
            end   = (i+1) * batch_size
            batch = {}
            for key in self.keys:
                batch[key] = self.seg[key][begin: end]
            yield batch

def add_advs_and_rets(seg, gamma, lam):
    T = len(seg["rews"])
    rews = seg["rews"]
    news = np.append(seg["news"], 0)
    vals = np.append(seg["vals"], seg["nval"])
    seg["advs"] = gaelam = np.empty(T, np.float32)
    lastgaelam = 0
    for t in reversed(range(T)):
        nonterminal = 1 - news[t+1]
        delta = rews[t] + gamma * vals[t+1] * nonterminal - vals[t]
        gaelam[t] = lastgaelam = delta + gamma * lam * nonterminal * lastgaelam
    seg["rets"] = seg["advs"] + seg["vals"]
    seg["advs"] -= seg["advs"].mean()
    seg["advs"] /= seg["advs"].std()

ef = None

def rollout_worker(exp_queue, info_queue, i):
    info = info_queue.get()
    logstd = info["logstd"]
    HORIZON = info["HORIZON"]
    MODEL_PATH = info["MODEL_PATH"]
    NUM_HIDDEN = info["NUM_HIDDEN"]
    HIDDEN_SIZE = info["HIDDEN_SIZE"]
    env_function = info["env_function"]
    RESTORE_INTERVAL = info["RESTORE_INTERVAL"]
    # env_function = CompetitiveSnake
    env = env_function(index = i, gui=GUI)
    obv_space = env.obv_space()
    # obv_ = tf.placeholder(tf.float32, shape=obv_space[0])
    obv_ = tf.placeholder(tf.float32, shape=[None, obv_space])

    action_space = env.act_space()
    # action_ = tf.placeholder(tf.float32, shape=action_space[0])
    action_ = tf.placeholder(tf.float32, shape=[None, action_space])
    policy = ContinuousPolicy(obv_, action_, NUM_HIDDEN, HIDDEN_SIZE, scope="new")

    saver = tf.train.Saver()
    sess = tf.Session()
    saver.restore(sess, MODEL_PATH)
    last_restore_time = time()

    t = 0
    act1 = np.zeros(action_space)
    act2 = np.zeros(action_space)
    new1 = False
    new2 = False
    # obv1, _, _, obv2, _, _ = env.step(act1, act2)
    obv1, obv2 = env.reset()

    obvs1 = np.array([obv1 for _ in range(HORIZON)])
    obvs2 = np.array([obv2 for _ in range(HORIZON)])
    acts1 = np.array([act1 for _ in range(HORIZON)])
    acts2 = np.array([act2 for _ in range(HORIZON)])

    rews1 = np.zeros(HORIZON, np.float32)
    rews2 = np.zeros(HORIZON, np.float32)
    vals1 = np.zeros(HORIZON, np.float32)
    vals2 = np.zeros(HORIZON, np.float32)
    news1 = np.zeros(HORIZON, np.int32)
    news2 = np.zeros(HORIZON, np.int32)

    action = policy.stochastic_action

    while True:
        if time() - last_restore_time > RESTORE_INTERVAL:
            saver.restore(sess, MODEL_PATH)
            last_restore_time = time()

        try:
            info = info_queue.get(block=False)
            logstd = info["logstd"]
        except: pass

        act1, val1 = sess.run(
            [action, policy.value],
            feed_dict = {
                policy.obv_: obv1[None],
                policy.logstd: logstd
            }
        )

        act2, val2 = sess.run(
            [action, policy.value],
            feed_dict = {
                policy.obv_: obv2[None],
                policy.logstd: logstd
            }
        )

        act1 = act1[0]
        act2 = act2[0]
        val1 = val1[0]
        val2 = val2[0]
        nval1 = val1 * (1 - new1)
        nval2 = val2 * (1 - new2)

        if t > 0 and t % HORIZON == 0:
            exp_queue.put({
                "obvs" : obvs1,
                "rews" : rews1,
                "news" : news1,
                "acts" : acts1,
                "vals" : vals1,
                "nval" : nval1
            })
            exp_queue.put({
                "obvs" : obvs2,
                "rews" : rews2,
                "news" : news2,
                "acts" : acts2,
                "vals" : vals2,
                "nval" : nval2
            })

        i = t % HORIZON
        obvs1[i] = obv1
        obvs2[i] = obv2
        news1[i] = new1
        news2[i] = new2
        acts1[i] = act1
        acts2[i] = act2
        vals1[i] = val1
        vals2[i] = val2

        obv1, rew1, new1, obv2, rew2, new2 = env.step(act1, act2)
        rews1[i] = rew1
        rews2[i] = rew2

        if (new1):
            obv1, obv2 = env.reset()

        t += 1

def learn(env_function):
    logstd = MAX_LOGSTD

    envp = env_function(index = 0, nostart=True)

    obv_space = envp.obv_space()
    act_space = envp.act_space()

    obv_ = tf.placeholder(tf.float32, shape=[None, obv_space])
    act_ = tf.placeholder(tf.float32, shape=[None, act_space])
    # obv_ = tf.placeholder(tf.float32, shape=obv_space)
    # act_ = tf.placeholder(tf.float32, shape=act_space)

    adv_ = tf.placeholder(tf.float32, shape=[None])
    ret_ = tf.placeholder(tf.float32, shape=[None])

    new_policy = ContinuousPolicy(obv_, act_, NUM_HIDDEN, HIDDEN_SIZE, scope="new")
    old_policy = ContinuousPolicy(obv_, act_, NUM_HIDDEN, HIDDEN_SIZE, scope="old")

    nlp = new_policy.dist.logp(act_)
    olp = old_policy.dist.logp(act_)

    pairs = zip(old_policy.variables, new_policy.variables)
    assign_old = [tf.assign(old, new) for old, new in pairs]
    saver = tf.train.Saver(var_list=new_policy.variables)

    ratio = tf.exp(nlp - olp)
    surr1 = ratio * adv_
    surr2 = tf.clip_by_value(ratio, 1-CLIP, 1+CLIP) * adv_
    pol_loss = -tf.reduce_mean(tf.minimum(surr1, surr2))
    val_loss = tf.reduce_mean(tf.square(new_policy.value - ret_))
    total_loss = pol_loss + val_loss
    train_step = tf.train.AdamOptimizer(1e-5, epsilon=1e-5).minimize(total_loss)

    sess = tf.Session()
    sess.run(tf.global_variables_initializer())
    saver.save(sess, MODEL_PATH)
    last_save_time = time()

    info_queues = {}
    exp_queue = Queue(maxsize=QUEUE_SIZE)
    for i in range(NUM_WORKERS):
        info_queues[i] = Queue(maxsize=1)
        info_queues[i].put({
            "logstd": logstd,
            "HORIZON": HORIZON,
            "MODEL_PATH": MODEL_PATH,
            "NUM_HIDDEN": NUM_HIDDEN,
            "HIDDEN_SIZE": HIDDEN_SIZE,
            "env_function": env_function,
            "RESTORE_INTERVAL": RESTORE_INTERVAL
        })
        Process(
            target=rollout_worker,
            args=(exp_queue, info_queues[i], i)
        ).start()

    logstd_step_size = (MAX_LOGSTD - MIN_LOGSTD) / NUM_LOGSTD_STEPS

    logger = Logger([
        "MeanAbsReward",
        "Mean",
        "Std",
        "ValueLoss",
        "AverageTime"
    ])
    total = 0
    for i in range(NUM_ITERATIONS):
        start = time()

        if time() - last_save_time > RESTORE_INTERVAL:
            saver.save(sess, MODEL_PATH)
            last_save_time = time()
            print("saved, iter:", i)

        logstd = MAX_LOGSTD - logstd_step_size * i
        logstd = np.maximum(logstd, MIN_LOGSTD)
        for queue in info_queues:
            try: queue.put({"logstd": logstd}, block=False)
            except: pass

        seg = exp_queue.get()
        mean_abs_reward = np.mean(np.abs(seg["rews"]))
        add_advs_and_rets(seg, GAMMA, LAMBDA)
        dataset = Dataset(seg, ["obvs", "acts", "advs", "rets"])
        sess.run(assign_old)
        mean, std, loss, count = 0, 0, 0, 0
        for _ in range(EPOCHS):
            for batch in dataset.iterate(BATCH_SIZE):
                _, m, s, l = sess.run(
                    [train_step,
                     new_policy.dist.mean,
                     new_policy.dist.std,
                     val_loss],
                    feed_dict = {
                        obv_ : batch["obvs"],
                        act_ : batch["acts"],
                        adv_ : batch["advs"],
                        ret_ : batch["rets"],
                        old_policy.logstd : logstd,
                        new_policy.logstd : logstd
                    })
                mean += np.mean(m)
                std  += np.mean(s)
                loss += np.mean(l)
                count += 1

        mean /= count
        std  /= count
        loss /= count

        total += time() - start
        avg_time = total / (i+1)

        logger.add_data("MeanAbsReward", mean_abs_reward)
        logger.add_data("Mean", mean)
        logger.add_data("Std" , std )
        logger.add_data("ValueLoss", loss)
        logger.add_data("AverageTime", avg_time)
        logger.display()
