#pragma once

#include "vector.hpp"

#include <algorithm>

namespace math {

template <typename T>
using LineSeg = std::pair<Vec<T>, Vec<T>>;

/* Returns:
 *   > 0 if C is left of AB
 *   < 0 if C is right of AB
 *   = 0 if C is collinear with AB
 */
template <typename T>
T orient(const LineSeg<T>& AB, Vec<T> C)
{
    const Vec<T>& A = AB.first;
    const Vec<T>& B = AB.second;
    return (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
}

// Check if P lies on segment AB (assuming collinear)
template <typename T>
bool onSegment(LineSeg<T> AB, Vec<T> P)
{
    const Vec<T>& A = AB.first;
    const Vec<T>& B = AB.second;
    return (P.x >= std::min(A.x, B.x) && P.x <= std::max(A.x, B.x)
        && P.y >= std::min(A.y, B.y)  && P.y <= std::max(A.y, B.y));
}

template <typename T>
bool segmentsIntersect(const LineSeg<T> AB, const LineSeg<T> CD)
{
    const Vec<T>& A = AB.first;
    const Vec<T>& B = AB.second;
    const Vec<T>& C = CD.first;
    const Vec<T>& D = CD.second;

    const T d1 = orient(CD, A);
    const T d2 = orient(CD, B);
    const T d3 = orient(AB, C);
    const T d4 = orient(AB, D);

    // Proper intersection (straddling)
    if (((d1 > 0 and d2 < 0) || (d1 < 0 and d2 > 0)) && ((d3 > 0 and d4 < 0) || (d3 < 0 and d4 > 0)))
        return true;

    // Collinear cases (need to check bounding boxes)
    if ((d1 == 0) && onSegment(CD, A))
        return true;
    if ((d2 == 0) && onSegment(CD, B))
        return true;
    if ((d3 == 0) && onSegment(AB, C))
        return true;
    if ((d4 == 0) && onSegment(AB, D))
        return true;

    return false;
}
}
