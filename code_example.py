points = [(0,0,1)]
for a in field:
    points.append((0,1,a))
    for b in field:
        points.append((1,a,b))