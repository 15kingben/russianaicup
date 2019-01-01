import multiprocessing
from CompetitivePPO import learn
from CompetitiveSnake import CompetitiveSnake
from codeball_env import CodeballEnvVanilla

if __name__ == "__main__":
    multiprocessing.set_start_method("spawn")
    learn(CodeballEnvVanilla)
