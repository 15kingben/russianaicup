import json
import pandas as pd

SAVE_PERIOD = 10

class Logger(object):

    def __init__(self, columns):
        self.ctr = 0
        self.columns = columns
        self.data = {}
        for column in columns:
            self.data[column] = []

    def add_data(self, name, data):
        self.data[name].append(data)

    def display(self):
        self.ctr += 1
        result = ""
        for index, key in enumerate(self.columns):
            result += key
            result += ": "
            result += "{:.3f}".format(self.data[key][-1])
            result += "   "
        print(result)
        if self.ctr % SAVE_PERIOD == 0:
            pd.DataFrame(self.data).to_csv("log/log.csv", index=False)
            self.ctr = 0

        

            
