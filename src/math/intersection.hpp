def orient(A, B, C):
    """Returns:
    > 0 if C is left of AB
    < 0 if C is right of AB  
    = 0 if C is collinear with AB
    """
    return (B[0]-A[0])*(C[1]-A[1]) - (B[1]-A[1])*(C[0]-A[0])

def segments_intersect(A, B, C, D):
    d1 = orient(C, D, A)
    d2 = orient(C, D, B)
    d3 = orient(A, B, C)
    d4 = orient(A, B, D)
    
    # Proper intersection (straddling)
    if ((d1 > 0 and d2 < 0) or (d1 < 0 and d2 > 0)) and \
       ((d3 > 0 and d4 < 0) or (d3 < 0 and d4 > 0)):
        return True
    
    # Collinear cases (need to check bounding boxes)
    if d1 == 0 and on_segment(C, D, A): return True
    if d2 == 0 and on_segment(C, D, B): return True
    if d3 == 0 and on_segment(A, B, C): return True
    if d4 == 0 and on_segment(A, B, D): return True
    
    return False

def on_segment(A, B, P):
    """Check if P lies on segment AB (assuming collinear)"""
    return (min(A[0], B[0]) <= P[0] <= max(A[0], B[0]) and
            min(A[1], B[1]) <= P[1] <= max(A[1], B[1]))
