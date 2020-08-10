# Lichtenberg
from lichtenberg.util import draw_blur

# Standard Libraries
from typing import List, Tuple, Callable
from random import randint
import tkinter as tk
from tkinter.filedialog import asksaveasfilename
import sys

# External Libraries
from PIL import Image, ImageTk


class Window:
    """
    Widgets and Drawing
    """
    def __init__(self, **kwargs):
        self.image = None
        self.root = tk.Tk()
        title = kwargs.get("title", "tk App")
        self._init_components(title)

    def _init_components(self, title):
        self.root.title(title)
        self.root.resizable(False, False)
        # self.root.bind("<Escape>", self.key_event)

        self._init_left_frame()
        self._init_right_frame()
        self._init_menu()

    def _init_left_frame(self):
        frame = tk.Frame(self.root, bd=2)

        # Upper - List Box
        box = tk.Listbox(frame, selectmode=tk.SINGLE, width=30, bd=2, font=("", 12))
        box.pack(fill=tk.BOTH, expand=1, side=tk.TOP)
        box.bind("<<ListboxSelect>>", self.listbox_event)
        self.listbox = box

        # Lower - Frame
        self._init_left_info_frame(frame)

        frame.pack(side=tk.LEFT, fill=tk.Y, expand=1)

    def _init_left_info_frame(self, parent):
        frame = tk.Frame(parent, bd=2)

        def callback(name):
            return lambda v: self.scale_event(name, v)

        def add_scale(label, id_, from_, to):
            f = tk.Frame(frame)
            tk.Label(f, text=label).grid(column=0, row=0)
            scale = tk.Scale(f, from_=from_, to=to, length=150, resolution=0.01, orient=tk.HORIZONTAL, command=callback(id_))
            scale.grid(column=1, row=0)
            f.pack()
            return scale

        self.weight = add_scale("Weight:", "weight", 0.01, 2.00)
        self.col_r = add_scale("Color-R:", "col-r", 0.01, 2.00)
        self.col_g = add_scale("Color-G:", "col-g", 0.01, 2.00)
        self.col_b = add_scale("Color-B:", "col-b", 0.01, 2.00)
        tk.Label(frame, text="Update manually with Ctrl+R after changes").pack()

        frame.pack(side=tk.BOTTOM, fill=tk.Y)

    def _init_right_frame(self):
        # Right - Image Label
        label = tk.Label(self.root)
        label.bind("<Button-1>", self.mouse_event)  # Mouse-Left
        label.bind("<Button-3>", self.mouse_event)  # Mouse-Right
        label.pack(side=tk.RIGHT)
        self.label = label

    def _init_menu(self):
        # Menu
        def callback(name):
            return lambda: self.menu_event(name)

        # File Menu
        menubar = tk.Menu(self.root)
        file_menu = tk.Menu(menubar, tearoff=0)
        file_menu.add_command(label='Export as PNG', command=callback("file.export"))
        file_menu.add_separator()
        file_menu.add_command(label='Exit', command=callback("file.exit"))
        menubar.add_cascade(label='File', menu=file_menu)
        # Edit
        edit_menu = tk.Menu(menubar, tearoff=0)
        edit_menu.add_command(label='New Line', command=callback("edit.new"), accelerator="Right Button")
        edit_menu.add_command(label='Remove Line', command=callback("edit.remove"), accelerator="Del")
        edit_menu.add_command(label='Redraw with another seed', command=callback("edit.redraw"), accelerator="Ctrl+R")
        edit_menu.add_separator()
        edit_menu.add_command(label='Remove Last Point', command=callback("edit.back"), accelerator="Backspace")
        menubar.add_cascade(label='Edit', menu=edit_menu)
        self.root.config(menu=menubar)

        self.root.bind_all("<Delete>", self.key_event)
        self.root.bind_all("<BackSpace>", self.key_event)
        self.root.bind_all("<Control-r>", self.key_event)

    def change_image(self, new_image: Image):
        self.image = ImageTk.PhotoImage(new_image)
        self.label.configure(image=self.image)

    def update_scales(self, weight, r, g, b):
        self.weight.set(weight)
        self.col_r.set(r)
        self.col_g.set(g)
        self.col_b.set(b)

    # Key Events
    def key_event(self, event):
        pass

    # Mouse Events
    def mouse_event(self, event):
        pass

    # ListBox Events
    def listbox_event(self, event):
        pass

    # Menu
    def menu_event(self, name):
        pass

    # Scale
    def scale_event(self, name, value):
        pass

    def run(self):
        self.root.mainloop()


class Line:
    def __init__(self):
        self.points: List[Tuple[int, int]] = []
        self.weight = 0.50
        self.col_r = 1.00
        self.col_g = 1.00
        self.col_b = 1.00
        self.seed = randint(0, 2**16)

    def add(self, point):
        self.points.append(point)

    def back(self):
        try:
            self.points.pop()
        except IndexError:
            pass

    @property
    def num_points(self):
        return len(self.points)

    @property
    def color(self):
        return self.col_r, self.col_g, self.col_b

    def update_seed(self):
        self.seed = randint(0, 2**16)

    def __str__(self):
        n = len(self.points)
        w = self.weight
        f = "" if n == 0 else str(self.points[0])
        return f"points:{n} weight={w} {f}"


class LineListManager:
    """
    Synchronization between Line-data and ListBox
    """
    def __init__(self, listbox, callback_on_select: Callable[[Line], None]):
        self.listbox = listbox
        self.data: List[Line] = []
        self.current_index = -1
        self.callback_on_select = callback_on_select

    @property
    def current_line(self):
        if self.current_index < 0:
            return None
        else:
            return self.data[self.current_index]

    @property
    def line_count(self):
        return len(self.data)

    def new_line(self):
        # add new line
        self.data.append(Line())
        self.current_index = self.line_count - 1
        self.callback_on_select(self.current_line)
        # sync with listbox
        self._clear_all_selection()
        self.listbox.insert(tk.END, str(self.current_line))
        self.listbox.selection_set(first=self.current_index)
        return False

    def remove_line(self):
        if self.current_line:
            self._remove_line(self.current_index)
            self._select_line(self.current_index)
            return True
        else:
            return False

    def add_point(self, point):
        if self.current_line is None:
            self.new_line()
        self.current_line.add(point)
        self._update_listbox()
        update_require = self.current_line.num_points >= 2
        return update_require

    def remove_point(self):
        if self.current_line:
            self.current_line.back()
            self._update_listbox()
            return True
        else:
            return False

    def update_seed(self):
        if self.current_line:
            self.current_line.update_seed()
            return True
        else:
            return False

    def _clear_all_selection(self):
        for i in self.listbox.curselection():
            self.listbox.selection_clear(first=i)

    def _update_listbox(self):
        self._clear_all_selection()
        self.listbox.delete(self.current_index)
        self.listbox.insert(self.current_index, str(self.current_line))
        self.listbox.select_set(first=self.current_index)

    def _remove_line(self, index):
        self.data.pop(index)
        self.listbox.delete(index)

    def _select_line(self, index):
        self._clear_all_selection()
        n = self.line_count
        if 0 <= index < n:
            self.current_index = index
            self.listbox.selection_set(first=self.current_index)
            self.callback_on_select(self.current_line)
        elif n > 0:
            self.current_index = n - 1
            self.listbox.selection_set(first=self.current_index)
            self.callback_on_select(self.current_line)
        else:
            self.current_index = -1


class App(Window):
    """
    Event Dispatching and Image generation
    """

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.base_image = self._init_image(**kwargs)
        initial_image = self.base_image.copy()
        self.change_image(initial_image)
        self.result_image = initial_image
        self.lines = LineListManager(self.listbox, self._callback_on_select)

    def _init_image(self, **kwargs):
        filepath = kwargs.get("filepath", None)
        if filepath:
            image = Image.open(filepath)
        else:
            width = kwargs.get("width", 640)
            height = kwargs.get("height", 480)
            image = Image.new("RGB", (width, height))
        return image

    def _update(self):
        self._generate_image()

    def key_event(self, event):
        update = False
        if event.keycode == 0x1b:  # Escape
            self.menu_event("file.exit")
        elif event.keycode == 0x2e:  # Delete
            self.menu_event("edit.remove")
        elif event.keycode == 0x08:  # Backspace
            self.menu_event("edit.back")
        elif event.keycode == 0x52:  # Ctrl+R
            self.menu_event("edit.redraw")
        if update:
            self._update()

    def mouse_event(self, event):
        update = False
        if event.num == 1:
            update = self.lines.add_point((event.x, event.y))
        elif event.num == 3:
            update = self.lines.new_line()
        if update:
            self._update()

    def listbox_event(self, event):
        selections = self.listbox.curselection()
        if len(selections) > 0:
            self.lines.current_index = selections[0]
            self._callback_on_select(self.lines.current_line)

    def menu_event(self, name):
        update = False
        if name == "file.export":
            self._export_png()
        elif name == "file.exit":
            self.root.quit()
        elif name == "edit.new":
            update = self.lines.new_line()
        elif name == "edit.remove":
            update = self.lines.remove_line()
        elif name == "edit.redraw":
            update = self.lines.update_seed()
        elif name == "edit.back":
            update = self.lines.remove_point()
        if update:
            self._update()

    def scale_event(self, name, value):
        if self.lines.current_line:
            value = float(value)
            if name == "weight":
                self.lines.current_line.weight = value
            elif name == "col-r":
                self.lines.current_line.col_r = value
            elif name == "col-g":
                self.lines.current_line.col_g = value
            elif name == "col-b":
                self.lines.current_line.col_b = value

    def _callback_on_select(self, line: Line):
        # The selected line is changed. Renew values of scales
        self.weight.set(line.weight)
        self.col_r.set(line.col_r)
        self.col_g.set(line.col_g)
        self.col_b.set(line.col_b)

    def _generate_image(self):
        image = self.base_image.copy()
        blur_params = [(0, 1), (1, 4), (1, 8), (2, 8)]
        for line in self.lines.data:
            if len(line.points) >= 2:
                draw_blur(image, line.points, blur_params, line.weight, line.color, line.seed)
        self.change_image(image)
        self.result_image = image

    def _export_png(self):
        filepath = asksaveasfilename(initialdir=".", title="Export as PNG",
                                     filetypes=[("Portable Network Graphics(PNG) file", "*.png")],
                                     defaultextension=".png")
        if filepath == "":
            return
        else:
            self.result_image.save(filepath)


def main():
    kwargs = {"title": "Thunder Painter"}
    if len(sys.argv) == 2:
        # Open with Base Image
        base_image_path = sys.argv[1]
        kwargs["filepath"] = base_image_path
    elif len(sys.argv) == 3:
        # Open with Image Size
        kwargs["width"] = int(sys.argv[1])
        kwargs["height"] = int(sys.argv[2])
    app = App(**kwargs)
    app.run()


if __name__ == "__main__":
    main()
