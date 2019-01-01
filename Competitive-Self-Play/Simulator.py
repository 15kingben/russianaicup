import sys
import numpy as np
import tensorflow as tf
from CompetitiveSnake import CompetitiveSnake
from CompetitivePPO import ContinuousPolicy, NUM_HIDDEN, HIDDEN_SIZE

if __name__ == "__main__":
    env = CompetitiveSnake(gui=True)
    obv_space = CompetitiveSnake.obv_space()
    obv_ = tf.placeholder(tf.float32, shape=[None, obv_space])
    policy = ContinuousPolicy(obv_, NUM_HIDDEN, HIDDEN_SIZE, scope="new")
    saver = tf.train.Saver()
    sess = tf.Session()
    saver.restore(sess, sys.argv[1])
    obv1, _, _, obv2, _, _ = env.step(0.0, 0.0)
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
        obv1, rew1, new1, obv2, rew2, new2 = env.step(act1, act2)



    
