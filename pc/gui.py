import os
import ctypes
import customtkinter as ctxt

script_dir = os.path.dirname(os.path.abspath(__file__))
library_path = os.path.join(script_dir, "libsender.so")
lib = ctypes.CDLL(library_path)

lib.send_array.argtypes = [ctypes.POINTER(ctypes.c_uint32)]
lib.send_array.restype = ctypes.c_int

matrix_rows = [0] * 8

class MatrycaApp(ctxt.CTk):
    def __init__(self):
        super().__init__()

        self.title("Max7219 app")
        self.resizable(False, False)

        self.ROWS = 8
        self.COLS = 32

        self.button_fields = {}

        self.COLOR_DEFAULT = "#4A4A4A"
        self.COLOR_HOVER_DEFAULT = "#5A5A5A"
        
        self.COLOR_ACTIVE = "#FF0000"
        self.COLOR_HOVER_ACTIVE = "#CC0000"

        for r in range(self.ROWS):
            for c in range(self.COLS):
                self.button_fields[(r, c)] = False

                btn = ctxt.CTkButton(
                    self,
                    text="",
                    width=22,
                    height=22,
                    corner_radius=4,
                    fg_color=self.COLOR_DEFAULT,
                    hover_color=self.COLOR_HOVER_DEFAULT
                )

                btn.configure(command=lambda row=r, col=c, button=btn: self.on_click(row, col, button))
                btn.grid(row=r, column=c, padx=2, pady=2)

        self.upload_btn = ctxt.CTkButton(
            self,
            text="Upload",
            width=100,
            command=self.on_upload
        )
        self.upload_btn.grid(row=self.ROWS, column=self.COLS-6, columnspan=6, sticky="e", pady=12, padx=2)

    def on_click(self, r, c, btn):
        global matrix_rows
        current_state = not self.button_fields[(r, c)]
        self.button_fields[(r, c)] = current_state

        bit_shift = 31 - c

        if current_state:
            btn.configure(fg_color=self.COLOR_ACTIVE, hover_color=self.COLOR_HOVER_ACTIVE)
            matrix_rows[r] |= (1 << bit_shift)
        else:
            btn.configure(fg_color=self.COLOR_DEFAULT, hover_color=self.COLOR_HOVER_DEFAULT)
            matrix_rows[r] &= ~(1 << bit_shift)


    def on_upload(self):
        global matrix_rows
        for idx, val in enumerate(matrix_rows):
            print(f"[{idx}] = {val}")

        c_array = (ctypes.c_uint32 * 8)(*matrix_rows)
        result = lib.send_array(c_array)
        
        if result == -1:
            print("Error: send_array ")


if __name__ == "__main__":
    app = MatrycaApp()
    app.mainloop()