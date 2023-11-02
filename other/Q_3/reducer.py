# Hardcoded candidate points
candidates = [
    {"id": 0, "coords": [5.8, 4.0]},
    {"id": 1, "coords": [6.1, 2.8]},
    {"id": 2, "coords": [6.3, 2.7]}
]

def euclidean_distance(a, b):
    return sum((e1-e2)**2 for e1, e2 in zip(a,b)) ** 0.5

def mapper(line):
    point = line.split(',')
    coords = [float(point[0]), float(point[1])]

    # Find the closest candidate point
    min_distance = float('inf')
    closest_candidate_id = None
    for candidate in candidates:
        distance = euclidean_distance(coords, candidate["coords"])
        if distance < min_distance:
            min_distance = distance
            closest_candidate_id = candidate["id"]

    yield closest_candidate_id, coords
