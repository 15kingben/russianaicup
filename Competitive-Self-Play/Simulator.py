import sys
import numpy as np
import tensorflow as tf
from CompetitiveSnake import CompetitiveSnake
from CompetitivePPO import ContinuousPolicy, NUM_HIDDEN, HIDDEN_SIZE
from codeball_env import CodeballEnvVanilla

if __name__ == "__main__":
    env = CodeballEnvVanilla(index=30, gui=True)
    obv_space = env.obv_space()
    obv_ = tf.placeholder(tf.float32, shape=[None, obv_space])
    action_space = env.act_space()
    action_ = tf.placeholder(tf.float32, shape=[None, action_space])
    policy = ContinuousPolicy(obv_, action_, NUM_HIDDEN, HIDDEN_SIZE, scope="new")
    saver = tf.train.Saver()
    sess = tf.Session()
    saver.restore(sess, "models/policycb")
    obv1, obv2 = env.reset()
    action = policy.deterministic_action
    while True:
        act1 = sess.run(
            action,
            feed_dict = {
                policy.obv_: obv1[None],
                policy.logstd: np.log(0.5)
            }
        )
        act2 = sess.run(
            action,
            feed_dict = {
                policy.obv_: obv2[None],
                policy.logstd: np.log(0.5)
            }
        )
        obv1, rew1, new1, obv2, rew2, new2 = env.step(act1[0], act2[0])
        if new1:
            break
