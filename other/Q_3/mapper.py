def reducer(candidate_id, list_of_points):
    total_points = len(list_of_points)
    sum_coords = [sum(x) for x in zip(*list_of_points)]
    avg_coords = [coord/total_points for coord in sum_coords]
    yield candidate_id, avg_coords
