from agent import *
import gridworld as gw

class RandomWalk(Agent):
    def __init__(self, world):
        Agent.__init__(self)
        self.gw = world
    index = 0;
    last = None;
    current = 0;
    def points_to_int(data):
        for i in range(len(data)):
            for j in range(len(data[i])):
                data[i][j]=int(data[i][j])
    log = [x.split() for x in open("../log.txt").readlines()]
    paths = [x.split() for x in open("../paths.txt").readlines()]
    opens = [x.split() for x in open("../opens.txt").readlines()]
    closes = [x.split() for x in open("../closes.txt").readlines()]
    points_to_int(log)
    points_to_int(paths)
    points_to_int(opens)
    points_to_int(closes)

    def do_step(self, S, act, logfile=None):
        log = self.log
        paths = self.paths
        opens = self.opens
        closes = self.closes
        def draw_tiles(name, index, TILE):
            for i in range(len(name[index-1])):
                if self.gw.tiles[name[index-1][i]]==TILE:
                    self.gw.tiles[name[index-1][i]] = 0
            for i in range(len(name[index])):
                self.gw.tiles[name[index][i]] = TILE
        def action(dif):
            if dif==1:
                return 0
            if dif==-1:
                return 2
            if dif<0:
                return 1
            return 3
        Agent.do_step(self, S, act)
        size = (log[0][1]+1)**0.5
        if(not self.index):
            self.index = 1;
            self.last = log[0][0]
            self.gw.tiles[log[self.index-1][1]] = 0
        self.current = log[self.index][0]
        way = self.current-self.last
        self.last = self.current
        newgoal = log[self.index][1]
        draw_tiles(paths, self.index, gw.TILE_PATH)
        draw_tiles(opens, self.index, gw.TILE_OPEN)
        draw_tiles(closes, self.index, gw.TILE_CLOSED)

        self.gw.tiles[newgoal] = gw.TILE_GOAL
        self.gw.tiles[log[self.index-1][1]] = 0
        self.index+=1;
        R, Sp = act(action(way))

        self.G += R
