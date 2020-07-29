import unittest
import lichtenberg as lb


class TestSimulator(unittest.TestCase):
    def setUp(self):
        pass

    def test_invalid_size(self):
        with self.assertRaises(ValueError):
            lb.Simulator(0, 0)
        with self.assertRaises(ValueError):
            lb.Simulator(0, 1)
        with self.assertRaises(ValueError):
            lb.Simulator(1, 0)

    def test_valid_size(self):
        self.assertIsNotNone(lb.Simulator(1, 1))
        self.assertIsNotNone(lb.Simulator(1000, 1000))

    def test_run_without_breakdown(self):
        # At least one break point is needed
        sim = lb.Simulator(1, 1)
        with self.assertRaises(RuntimeError):
            sim.simulate()

    def test_run_with_breakdown(self):
        sim = lb.Simulator(1, 1)
        sim.breakdown(0, 0)
        sim.simulate()

    def test_breakdown(self):
        sim = lb.Simulator(1, 1)
        sim.breakdown(0, 0)
        self.assertTrue(sim.cells.get_broken(0, 0))

    def test_breakdown_invalid(self):
        # Invalid coordinate
        sim = lb.Simulator(1, 1)
        with self.assertRaises(ValueError):
            sim.breakdown(-1, 0)
        with self.assertRaises(ValueError):
            sim.breakdown(0, -1)
        with self.assertRaises(ValueError):
            sim.breakdown(1, 0)
        with self.assertRaises(ValueError):
            sim.breakdown(0, 1)

    def test_insulate(self):
        sim = lb.Simulator(1, 1)
        sim.insulate(0, 0)
        self.assertTrue(sim.cells.get_insulated(0, 0))

    def test_insulate_invalid(self):
        sim = lb.Simulator(1, 1)
        with self.assertRaises(ValueError):
            sim.insulate(-1, 0)
        with self.assertRaises(ValueError):
            sim.insulate(0, -1)
        with self.assertRaises(ValueError):
            sim.insulate(1, 0)
        with self.assertRaises(ValueError):
            sim.insulate(0, 1)

    def test_insulated_breakpoint(self):
        sim = lb.Simulator(2, 2)
        sim.insulate(1, 1)
        sim.breakdown(1, 1)  # invalid
        with self.assertRaises(RuntimeError):
            sim.simulate()
