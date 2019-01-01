class MyStrategy:
    def __init__(self, codeballRunner=None, team=1):
        self.manager = codeballRunner
        self.lowPlayer = None
        self.current_action = None
        self.team = team

    def act(self, me, rules, game, action):
        if self.current_action is not None and self.current_action[0] == game.current_tick:
            action_tuple = self.current_action[1]
        else:
            action_tuple = self.manager.get_action(me, rules, game, action, self.team)
            self.current_action = (game.current_tick, action_tuple)
        # modify action
        if self.lowPlayer is None:
            self.findLowPlayer(me, game)
        self.inputAction(action, action_tuple, self.lowPlayer == me.id)
        # self.debug_action(action)

    def custom_rendering(self):
        return ""

    def findLowPlayer(self, me, game):
        myId = me.id
        for robot in game.robots:
            if robot.is_teammate and robot.id != myId:
                otherId = robot.id
        self.isLowPlayer = myId if myId < otherId else otherId

    def inputAction(self, action, act_tup, is_low):
        offset = 0
        if is_low:
            offset = 4
        action.target_velocity_x = float(act_tup[0 + offset])
        action.target_velocity_y = float(act_tup[1 + offset])
        action.target_velocity_z = float(act_tup[2 + offset])
        action.jump_speed = float(act_tup[3 + offset])
        # action.jump_speed = 0.0
        action.use_nitro = False

    def debug_action(self, action):
        print("x:", action.target_velocity_x)
        print("y:", action.target_velocity_y)
        print("z:", action.target_velocity_z)
        print("jump:", action.jump_speed)

# class Action:
#     def __init__(self):
#         self.target_velocity_x = 0.0
#         self.target_velocity_y = 0.0
#         self.target_velocity_z = 0.0
#         self.jump_speed = 0.0
#         self.use_nitro = False
