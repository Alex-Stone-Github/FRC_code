/*
 * This experiment is written using the p5js framework
 */
// https://9rmjmr.csb.app/
class Segment {
  constructor(x, y, angle, l) {
    this.x = x;
    this.y = y;
    this.length = l;
    this.angle = angle;
  }
  go_to_target(x, y) {
    const angle_to_target = atan2(y - this.y, x - this.x); // y then x
    this.angle = angle_to_target;
    const current_endpoint = this.calculate_endpoint();
    this.x += x - current_endpoint.x;
    this.y += y - current_endpoint.y;
  }
  calculate_endpoint() {
    const endx = this.x + cos(this.angle) * this.length;
    const endy = this.y + sin(this.angle) * this.length;
    return {x: endx, y: endy};
  }
  show() {
    stroke(0);
    const end = this.calculate_endpoint();
    line(this.x, this.y, end.x, end.y);
    stroke(255);
    point(end.x, end.y);
  }
  translate(dx, dy) {
    this.x += dx;
    this.y += dy;
  }
}
class Arm {
  constructor(x, y, segment_lengths) {
    this.x = x;
    this.y = y;
    this.segments = [];
    let currentx = this.x;
    let currenty = this.y;
    for (const l of segment_lengths) {
      const new_segment = new Segment(currentx, currenty, -90, l);
      this.segments.push(new_segment);
      const new_current = new_segment.calculate_endpoint();
      currentx = new_current.x;
      currenty = new_current.y;
    }
    this.segments.reverse();
  }
  go_to_target(x, y) {
    let current_target_x = x;
    let current_target_y = y;
    for (let segment of this.segments) {
      segment.go_to_target(current_target_x, current_target_y);
      current_target_x = segment.x;
      current_target_y = segment.y;
    }
  }
  show() {
    for (const segment of this.segments) {
      segment.show();
    }
  }
  go_to_target_with_anchor(x, y) {
    const ITERATIONS = 10;
    const anchorx = this.x;
    const anchory = this.y;
    for (let i = 0; i < ITERATIONS; i ++) {
      this.go_to_target(x, y);
      const last_segment = this.segments[this.segments.length - 1]; // js needs a last method
      if (last_segment.x === anchorx && last_segment.y === anchory) {
        return; // the arm moved to a point without moving the anchor
      }
      // move back and prepare to retry
      const translationx = anchorx - last_segment.x;
      const translationy = anchory - last_segment.y;
      for (let segment of this.segments) {
        segment.translate(translationx, translationy);
      }
    }
  }
}

var arm;

function setup() {
  createCanvas(700, 700);
  arm = new Arm(400, 700, [130, 90])
}

function draw() {
  arm.go_to_target_with_anchor(mouseX, mouseY);
  
  strokeWeight(10);
  background(220);
  arm.show();
  stroke(255, 0, 0);
  point(mouseX, mouseY);
}
