import sys

from model import *
from MyStrategy import MyStrategy
from RemoteProcessClient import RemoteProcessClient


class Runner:
    def __init__(self, codeballRunner, port=31001, team=1):
        if sys.argv.__len__() == 4:
            self.remote_process_client = RemoteProcessClient(
                sys.argv[1], int(sys.argv[2]))
            self.token = sys.argv[3]
        else:
            self.remote_process_client = RemoteProcessClient(
                "127.0.0.1", port)
            self.token = "0000000000000000"
        self.manager = codeballRunner
        self.team = team

    def run(self):
        strategy = MyStrategy(self.manager, self.team)
        self.remote_process_client.write_token(self.token)
        rules = self.remote_process_client.read_rules()
        while True:
            game = self.remote_process_client.read_game()
            if game is None:
                break
            actions = {}

            for robot in game.robots:
                if robot.is_teammate:
                    action = Action()
                    strategy.act(robot, rules, game, action)
                    actions[robot.id] = action

            self.remote_process_client.write(actions, strategy.custom_rendering())
