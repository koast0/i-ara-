for i, row in enumerate(GRID):
    for j, room in enumerate(row):
        t=world[i][j].g
        f=world[i][j].f
        fval =FVal(world[i][j])
        fill = {
                 '.': 'white',
                 '#': 'black'
                 }[room]
        rect(40*j + 5, 40*i + 5, 40, 40, fill=fill, border='black')
        text(40*j + 30, 40*i + 40, str(t), size=10, font='Arial', color='black')
        text(40*j+7, 40*i+24, str(f), size=10, font='Arial', color='black')
        text(40*j + 18, 40*i + 20, str(fval), size=10, font='Arial', color='black')
for it in open:
    i = it[1].x;
    j = it[1].y
    text(40*j + 8, 40*i + 40, "O", size=10, font='Arial', color='black')
for it in closed:
    i = it[1].x;
    j = it[1].y
    text(40*j + 8, 40*i + 40, "C", size=10, font='Arial', color='black')
for it in incons:
    i = it[1].x;
    j = it[1].y
    text(40*j + 8, 40*i + 40, "I", size=10, font='Arial', color='black')
text(20, 400, "ARA* eps:"+str(eps), size=90)