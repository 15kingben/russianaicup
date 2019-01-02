# starts up codeball server
# sends commands from ml agents
from Runner import Runner
import threading
import time
import subprocess
import math

codeball_directory = "/home/ben/russianaicup/codeball2018-linux/codeball2018"
CODEBALL_GOAL_REWARD = 1
CODEBALL_DIS_BALL_REWARD = 0
CODEBALL_BALL_GOAL_REWARD = 0
CODEBALL_MAX_TICK_LIMIT = 5000 # 20000
CODEBALL_TIMEOUT_REWARD = 1
def d(x1, x2, x3, y1, y2, y3):
    return math.sqrt((y1-x1)**2 + (x2-y2)**2 + (x3-y3)**2)

class CodeballRunner:
    def __init__(self, index, gui):
        self.codeball_server = None
        self.port1 = 31000 + 2*index + 1
        self.port2 = 31000 + 2*index + 2
        self.gui = gui
        self.obs1 = None
        self.obs2 = None
        self.player1_action = None
        self.player2_action = None
        self.player1 = None
        self.player2 = None
        self.done = None
        self.reward1= None
        self.reward2 = None
        self.gameLock = threading.Lock()

    def run_server(self):
        # self.codeball_server = subprocess.call([codeball_directory, "--p1", "tcp-" + str(self.port1), "--p2", "tcp-" + str(self.port2)], shell=True)
        command = "exec " + codeball_directory + " --p1 tcp-" + str(self.port1) + " --p2 tcp-" + str(self.port2)
        if not self.gui:
            command += " --noshow"
        self.codeball_server = subprocess.Popen(command, shell=True)

    def run_player(self, port, team):
        x = Runner(self, port, team)
        x.run()
        print("shut down", team)

    # def monitor_game(self):
    #     while self.player1.isAlive() or self.player2.isAlive():
    #         time.sleep(1)
    #     if self.codeball_server is not None:
    #         self.codeball_server.terminate()

    def run_game(self):
        # Runner(self).run()
        self.player1 = threading.Thread(target=self.run_player, args=(self.port1, 1))
        self.player2 = threading.Thread(target=self.run_player, args=(self.port2, 2))
        self.player1.start()
        self.player2.start()
        # threading.Thread(target=self.monitor_game).start()

    def get_action(self, me, rules, game, action, team):
        # time.sleep(1)
        # if game.current_tick > 300 and not self.is_resetting:
        #     self.is_resetting = True
        #     self.reset()
        with self.gameLock:
            self.saveGameInfo(game, me, team)
        if team == 1:
            while True:
                with self.gameLock:
                    if self.onreset:
                        return (0,0,0,0,False,0,0,0,0,False)
                    if self.player1_action is not None:
                        ret = self.player1_action
                        self.player1_action = None
                        return ret
                # time.sleep(.1)
        if team == 2:
            while True:
                with self.gameLock:
                    if self.player2_action is not None:
                        ret = self.player2_action
                        self.player2_action = None
                        return ret
                # time.sleep(.1)

    def reset(self):
        if self.codeball_server is not None:
            self.codeball_server.kill()
            self.codeball_server = None
        if self.player1 is not None and self.player2 is not None:
            while self.player1.isAlive() or self.player2.isAlive():
                self.player1_action = (0,0,0,0,False,0,0,0,0,False)
                self.player2_action = (0,0,0,0,False,0,0,0,0,False)
        self.obs1 = None
        self.obs2 = None
        self.reward1 = None
        self.reward2 = None
        self.player1_action = None
        self.player2_action = None
        self.done = None
        self.gameLock = threading.Lock()
        self.onreset = True
        # threading.Thread(target=self.run_server).start()
        self.run_server()
        time.sleep(4)
        self.run_game()
        while True:
            with self.gameLock:
                if self.obs1 is not None and self.obs2 is not None:
                    ret1 = self.obs1
                    self.obs1 = None
                    ret2 = self.obs2
                    self.obs2 = None
                    self.onreset = False
                    return ret1, ret2
            # time.sleep(.1)

    def saveGameInfo(self, game, me, team):
        has_scored = False
        we_scored = False
        for player in game.players:
            if player.score != 0:
                has_scored = True
                if player.id == me.id:
                    we_scored = True
        if has_scored:
            print("ZOMG someone scored")
            with open("poop.txt", 'a') as f:
                f.write("poop\n")
            self.done = True
            m = 1 if (team == 1) == we_scored else -1

            self.reward1 = m*CODEBALL_GOAL_REWARD
            self.reward2 = -1*m*CODEBALL_GOAL_REWARD

            self.done = True
        elif game.current_tick > CODEBALL_MAX_TICK_LIMIT:
            self.done = True
            self.reward1 = -1*CODEBALL_TIMEOUT_REWARD
            self.reward2 = -1*CODEBALL_TIMEOUT_REWARD
        else:
            # distance ball goal
            mindus = 100000; mindthem = 100000
            for robot in game.robots:
                if robot.is_teammate:
                    mindus = min(mindus, d(game.ball.x, game.ball.y, game.ball.z, robot.x, robot.y, robot.z))
                else:
                    mindthem = min(mindthem, d(game.ball.x, game.ball.y, game.ball.z, robot.x, robot.y, robot.z))
            rew = CODEBALL_DIS_BALL_REWARD if mindus < mindthem else -1*CODEBALL_DIS_BALL_REWARD
            m = 1 if (team == 1) else -1
            self.reward1 = m*rew
            self.reward2 = -1*m*rew


            if abs(d(game.ball.x, game.ball.y, game.ball.z, 0,0,-50) - d(game.ball.x, game.ball.y, game.ball.z, 0,0,50)) > 5:
                if d(game.ball.x, game.ball.y, game.ball.z, 0,0,-50) - d(game.ball.x, game.ball.y, game.ball.z, 0,0,50) > 5:
                    rew = CODEBALL_BALL_GOAL_REWARD
                else:
                    rew = -1*CODEBALL_BALL_GOAL_REWARD
                self.reward1 += m*rew
                self.reward2 += -1*m*rew

            self.done = False

        teammates = [me.id]
        opponents = []
        tinfo = []
        oinfo = []
        for robot in game.robots:
            if robot.is_teammate or robot.id == me.id:
                tinfo += [robot.x, robot.y, robot.z, robot.velocity_x, robot.velocity_y, robot.velocity_z]
            else:
                oinfo += [robot.x, robot.y, robot.z, robot.velocity_x, robot.velocity_y, robot.velocity_z]
        binfo = [game.ball.x, game.ball.y, game.ball.z, game.ball.velocity_x, game.ball.velocity_y, game.ball.velocity_z]

        ginfo = tinfo + oinfo + binfo
        if team == 1:
            self.obs1 = ginfo
        else:
            self.obs2 = ginfo


    def simulate(self, actions):
        with self.gameLock:
            self.player1_action = actions[0]
            self.player2_action = actions[1]

    def get_obs(self):
        while True:
            with self.gameLock:
                if self.obs1 is not None and self.obs2 is not None:
                    ret1 = self.obs1
                    ret2 = self.obs2
                    self.obs1 = None
                    self.obs2 = None
                    return ret1, ret2
            # time.sleep(.1)

    def get_reward(self):
        while True:
            with self.gameLock:
                if self.reward1 is not None and self.reward2 is not None:
                    ret1 = self.reward1
                    ret2 = self.reward2
                    self.reward1 = None
                    self.reward2 = None
                    return ret1, ret2
            # time.sleep(.1)

    def get_done(self):
        while True:
            with self.gameLock:
                if self.done is not None:
                    ret = self.done
                    self.done = None
                    return ret
            # time.sleep(.1)
