import unittest
import typing
import dataclasses
import pygame
import abc

clamp = lambda x, lower, upper: max(min(x, upper), lower)
max_speed_mps = 5

class SimulationElement(abc.ABC):
    @abc.abstractmethod
    def update_simulation(self, dt):
        pass

class SimulatedMotionSystem(SimulationElement):
    encoder_value: float = 0
    motor_pwr: float = 0
    def update_simulation(self, dt):
        k = 0.27872398 # some weird constant based on the encoders default
        self.encoder_value += self.motor_pwr * k * dt
    def read(self):
        return self.encoder_value
    def read_with_conversion(self, ticks_per_unit: float):
        return self.read() / ticks_per_unit
    def write(self, power):
        self.motor_pwr = power

class PController: # proportional pid controller
    p: float = 1
    def __init__(self, pvalue: float):
        self.p = pvalue
    def calculate(self, error: float):
        return error * self.p

@dataclasses.dataclass
class SwerveCellState:
    wheel_radians: float = 0
    speed_mps: float = 0

class SwerveCell(SimulationElement):
    wheel: SimulatedMotionSystem = SimulatedMotionSystem()
    wheel_rotator: SimulatedMotionSystem = SimulatedMotionSystem()
    rotator_controller: PController = PController(.3)
    def set_state(self, new_state: SwerveCellState):
        # TODO: This is very important and could to some optimization in the rotate method to skip some stuff
        # wheel speed
        self.wheel.write(min(new_state.speed_mps, max_speed_mps)/max_speed_mps)
        # set the wheel angle
        current_state = self.get_state()
        error = new_state.wheel_radians - current_state.wheel_radians
        power = clamp(self.rotator_controller.calculate(error), -1, 1)
        self.wheel_rotator.write(power)
    def get_state(self):
        return SwerveCellState(
            speed_mps=self.wheel.motor_pwr*max_speed_mps,
            wheel_radians=self.wheel_rotator.read_with_conversion(.1)
        )
    def update_simulation(self, dt: float):
        self.wheel.update_simulation(dt)
        self.wheel_rotator.update_simulation(dt)


@dataclasses.dataclass
class Vec:
    x: float = 0
    y: float = 0

    def __add__(self, other):
        return Vec(self.x + other.x, self.y + other.y)

Color = (float, float, float)


class VectorTests(unittest.TestCase):
    def test_general(self):
        self.assertEqual(Vec(3, 4), Vec(2, 2) + Vec(1, 2))

front_left = SwerveCell()

def draw_vector(screen: pygame.Surface, color: Color, origin: Vec, vector: Vec):
    pygame.draw.circle(screen, color, (origin.x, origin.y), 4)
    pygame.draw.line(screen, color, (origin.x, origin.y), (origin.x + vector.x, origin.y + vector.y), 2)


class SwerveDriveKinematics:
    pass

#if __name__ == '__main__':
    #unittest.main()
#
# Graphics Stuff
#
pygame.init()
screen = pygame.display.set_mode([500, 500])
running = True
while running:
    # input
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    # update
    front_left.update_simulation(1)
    # draw
    screen.fill([200, 200, 200])
    draw_vector(screen, (255, 0, 0), Vec(200, 200), Vec(10, 20))
    pygame.display.flip()
pygame.quit()