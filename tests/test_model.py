import unittest
import lichtenberg as lb


class TestForImport(unittest.TestCase):
    def setUp(self):
        pass

    def test_default_model(self):
        model = lb.DefaultBreakModel()
        sim = lb.Simulator(1, 1, model)
        sim.breakdown(0, 0)
        sim.simulate()

    def test_value_noise_model_normal(self):
        model = lb.ValueNoiseBreakModel(1, 1)
        sim = lb.Simulator(1, 1, model)
        sim.breakdown(0, 0)
        sim.simulate()

    def test_value_noise_model_invalid(self):
        with self.assertRaises(ValueError):
            lb.ValueNoiseBreakModel(0, 0)
        with self.assertRaises(ValueError):
            lb.ValueNoiseBreakModel(0, 1)
        with self.assertRaises(ValueError):
            lb.ValueNoiseBreakModel(1, 0)

    def test_manual_model_normal(self):
        values = [[0.5]]
        model = lb.ManualBreakModel(1, 1, values)
        sim = lb.Simulator(1, 1, model)
        sim.breakdown(0, 0)
        sim.simulate()

    def test_manual_model_invalid(self):
        with self.assertRaises(ValueError):
            values = [[]]
            lb.ManualBreakModel(1, 1, values)
        with self.assertRaises(ValueError):
            values = [[0.5]]
            lb.ManualBreakModel(2, 1, values)
        with self.assertRaises(ValueError):
            values = [[0.5], []]
            lb.ManualBreakModel(1, 2, values)

