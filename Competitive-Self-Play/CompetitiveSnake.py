import subprocess
import numpy as np
from time import time

STATE_SIZE = 4 * 10 + 2 + 1
ACTION_REPEAT = 10

class CompetitiveSnake(object):

    def __init__(self, gui=False):
        if gui:
            self.proc = subprocess.Popen(
                "./bin/CompetitiveSnakePipeWithGui",
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE
            )
        else:
            self.proc = subprocess.Popen(
                "./bin/CompetitiveSnake",
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE
            )

    def _step(self, action1, action2):
        action1 = np.array(action1, np.float32)
        action2 = np.array(action2, np.float32)
        self.proc.stdin.write(action1.tobytes())
        self.proc.stdin.write(action2.tobytes())
        self.proc.stdin.flush()
        result = self.proc.stdout.read(4 * (2 * STATE_SIZE + 3))
        result = np.frombuffer(result, np.float32)
        state1  = result[:STATE_SIZE]
        reward1 = result[STATE_SIZE]
        state2  = result[STATE_SIZE + 1: 2 * STATE_SIZE + 1]
        reward2 = result[2 * STATE_SIZE + 1]
        done = bool(result[-1])
        return state1, reward1, done, state2, reward2, done

    def step(self, action1, action2):
        state1, state2, reward1, reward2, done = 0, 0, 0, 0, 0
        for _ in range(ACTION_REPEAT):
            s1, r1, d1, s2, r2, d2 = self._step(action1, action2)
            state1 = s1
            state2 = s2
            reward1 += r1
            reward2 += r2
            if d1:
                done = True
                break
        return state1, reward1, done, state2, reward2, done

    @staticmethod
    def obv_space():
        return STATE_SIZE

    @staticmethod
    def act_space():
        return 1

if __name__ == "__main__":
    game = CompetitiveSnake(gui=True)
    while True:
        start = time()
        state1, reward1, done, state2, reward2, done = game.step(0, 0)
        print("Time: {:f}  State1: {} Reward1: {:f} Done: {}".format(
            time() - start,
            state1,
            reward1,
            done))
