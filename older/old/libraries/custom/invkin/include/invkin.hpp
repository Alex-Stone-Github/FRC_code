#ifndef INVKIN_H
#define INVKIN_H

#include <vector>
#include <cmath>
#include <algorithm>

#define PI 3.141592654

struct Vector {
    double x, y;
    Vector(double x, double y): x(x), y(y) {}
};
struct Segment {
    double parent_angle;
    double length, angle;
    Vector position;

    Segment(Vector position, double length, double angle) 
        : length(length), angle(angle), position(position) {}
    Vector calculate_end() {
        return {
            position.x + cos(angle) * length,
            position.y + sin(angle) * length
        };
    }
    void move_to_target(Vector target) {
        angle = atan2(target.y - position.y, target.x - position.x);
        Vector current_end = calculate_end();
        position.x += target.x - current_end.x;
        position.y += target.y - current_end.y;
    }
};
class Arm {
private:
    std::vector<Segment> m_segments;
    Vector m_anchor;
public:
    Arm(Vector position, const std::vector<double>& segment_lengths)
    : m_anchor(position) {
        // generate the segments
        Vector current_segment_position = position;
        for (auto length : segment_lengths) {
            Segment new_segment(current_segment_position, length, -PI/2.0);
            m_segments.push_back(new_segment);
            Vector new_segment_end = new_segment.calculate_end();
            current_segment_position = new_segment_end;
        }
        // reverse the segment order
        std::reverse(m_segments.begin(), m_segments.end());
    }
    void move_to_target(Vector target) {
        Vector current_target = target;
        for (int i = 0; i < m_segments.size(); i ++) {
            m_segments[i].move_to_target(current_target);
            current_target = m_segments[i].position;
        }
    }
    void move_to_target_with_anchoring(Vector target) {
        const int ITERATIONS = 6;
        for (int i = 0; i < ITERATIONS; i ++) {
            move_to_target(target);
            Segment last_segment = m_segments[m_segments.size()-1];
            if (last_segment.position.x == m_anchor.x &&
                    last_segment.position.y == m_anchor.y) {
                return; // we got to the point without moving the anchor
            }
            // reset if we have not found a good way yet
            Vector translation_back(m_anchor.x-last_segment.position.x,
                    m_anchor.y-last_segment.position.y);
            for (int i = 0; i < m_segments.size(); i ++) {
                m_segments[i].position.x += translation_back.x;
                m_segments[i].position.y += translation_back.y;
            }
        }
    }
    std::vector<Segment> get_segments() {
        return std::vector<Segment>(m_segments);
    }
};

#endif // INVKIN_H



