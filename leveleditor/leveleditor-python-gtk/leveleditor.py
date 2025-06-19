#!/usr/bin/env python3
import gi

gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gdk, GdkPixbuf, GLib
import os
import re


class SDLBallLevelEditor:
    def __init__(self):
        # Level-Daten
        self.bricks = [[0, 0] for _ in range(598)]  # [powerup, brick-type]
        self.selected_type = "1"
        self.selected_powerup = "M"
        self.bricks_per_row = 26

        # Bildpfade laden
        self.brick_images = {
            "0": "blank.png",
            "1": "blue.png",
            "2": "yellow.png",
            "3": "cement.png",
            "4": "glass.png",
            "5": "green.png",
            "6": "steel.png",
            "7": "purple.png",
            "8": "white.png",
            "9": "invisible.png",
            "A": "red.png",
            "B": "explosive.png",
            "C": "doom.png"
        }
        self.pixbufs = {}
        self.load_pixbufs()

        # UI aufbauen
        self.build_ui()

    def load_pixbufs(self):
        # Lade alle Brick-Bilder
        for brick_id, img_file in self.brick_images.items():
            path = os.path.join("../gfx", img_file)
            if os.path.exists(path):
                self.pixbufs[brick_id] = GdkPixbuf.Pixbuf.new_from_file(path)
            else:
                print(f"Warnung: Bild {path} nicht gefunden")

    def build_ui(self):
        # Hauptfenster
        self.window = Gtk.Window(title="SDL-Ball Level-Editor")
        self.window.set_default_size(1200, 800)
        self.window.connect("destroy", Gtk.main_quit)

        # Layout
        main_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        self.window.add(main_box)

        # Toolbar
        toolbar = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=5)
        new_button = Gtk.Button(label="Neues Level")
        new_button.connect("clicked", self.new_level)
        toolbar.pack_start(new_button, False, False, 0)

        # Board zu Text / Text zu Board
        board_to_data = Gtk.Button(label="Board → Data")
        board_to_data.connect("clicked", self.board_to_data)
        toolbar.pack_start(board_to_data, False, False, 0)

        data_to_board = Gtk.Button(label="Data → Board")
        data_to_board.connect("clicked", self.data_to_board)
        toolbar.pack_start(data_to_board, False, False, 0)

        main_box.pack_start(toolbar, False, False, 0)

        # Hauptbereich: Grid für Level und Werkzeuge
        content_paned = Gtk.Paned(orientation=Gtk.Orientation.HORIZONTAL)
        main_box.pack_start(content_paned, True, True, 0)

        # Linker Bereich: Level und Textfeld
        # left_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        left_paned = Gtk.Paned(orientation=Gtk.Orientation.VERTICAL)

        # Level-Zeichenbereich
        self.level_grid = Gtk.Grid()
        self.level_grid.set_row_spacing(0)
        self.level_grid.set_column_spacing(0)

        level_scroll = Gtk.ScrolledWindow()
        level_scroll.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        level_scroll.add(self.level_grid)

        # left_box.pack_start(level_scroll, True, True, 0)

        # Textfeld für Level-Daten
        self.level_data = Gtk.TextView()
        self.level_data.set_monospace(True)
        self.level_data.set_wrap_mode(Gtk.WrapMode.NONE)
        self.level_buffer = self.level_data.get_buffer()

        text_scroll = Gtk.ScrolledWindow()
        text_scroll.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        text_scroll.add(self.level_data)
        # text_scroll.set_size_request(-1, 150)
        # left_box.pack_start(text_scroll, False, True, 0)

        # content_paned.add1(left_box)
        left_paned.add1(level_scroll)
        left_paned.add2(text_scroll)
        left_paned.set_position(600)
        content_paned.add1(left_paned)

        # Rechter Bereich: Werkzeuge
        tools_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        tools_box.set_margin_start(10)
        tools_box.set_margin_end(10)

        # Brick-Typen
        self.selected_brick_label = Gtk.Label()
        self.selected_brick_label.set_markup("<b>Ausgewählter Brick-Typ:</b>")
        tools_box.pack_start(self.selected_brick_label, False, False, 0)

        self.selected_brick_image = Gtk.Image()
        self.selected_brick_image.set_from_pixbuf(self.pixbufs.get(self.selected_type))
        tools_box.pack_start(self.selected_brick_image, False, False, 0)

        # Brick-Typ-Grid
        types_grid = Gtk.Grid()
        types_grid.set_row_spacing(5)
        types_grid.set_column_spacing(5)

        col, row = 0, 0
        for brick_id in "123456789ABC":
            button = Gtk.Button()
            img = Gtk.Image.new_from_pixbuf(self.pixbufs.get(brick_id))
            button.set_image(img)
            button.connect("clicked", self.on_brick_type_selected, brick_id)
            types_grid.attach(button, col, row, 1, 1)
            col += 1
            if col > 3:
                col = 0
                row += 1

        tools_box.pack_start(types_grid, False, False, 0)

        # Separator
        tools_box.pack_start(Gtk.Separator(orientation=Gtk.Orientation.HORIZONTAL), False, False, 10)

        # Powerup-Auswahl
        self.selected_powerup_label = Gtk.Label()
        self.selected_powerup_label.set_markup(f"<b>Ausgewählter Powerup:</b> {self.selected_powerup}")
        tools_box.pack_start(self.selected_powerup_label, False, False, 0)

        powerup_store = Gtk.ListStore(str, str)
        powerups = [
            ("M", "Random (2%)"),
            ("K", "Random (100%)"),
            ("J", "Random (10%)"),
            ("L", "Random (5%)"),
            ("N", "Random (1%)"),
            ("0", "No Powerup"),
            ("Q", "Random Evil (100%)"),
            ("1", "Extrude"),
            ("2", "Shrink"),
            ("4", "Glue"),
            ("5", "Multiball"),
            ("6", "Go-Thru"),
            ("B", "Explosive ball"),
            ("E", "Gravity"),
            ("F", "Big ball"),
            ("G", "Normal ball"),
            ("H", "Small ball"),
            ("I", "Aim"),
            ("O", "Extra Life"),
            ("P", "Gun"),
            ("R", "LaserSight")
        ]

        for p_id, p_name in powerups:
            powerup_store.append([p_id, f"{p_id} - {p_name}"])

        self.powerup_combo = Gtk.ComboBox.new_with_model(powerup_store)
        renderer = Gtk.CellRendererText()
        self.powerup_combo.pack_start(renderer, True)
        self.powerup_combo.add_attribute(renderer, "text", 1)
        self.powerup_combo.set_active(0)
        self.powerup_combo.connect("changed", self.on_powerup_changed)

        tools_box.pack_start(self.powerup_combo, False, False, 0)

        # Info-Text
        info_label = Gtk.Label()
        info_label.set_markup("<small>Der ausgewählte Powerup wird jedem Brick zugewiesen, den du platzierst.</small>")
        info_label.set_line_wrap(True)
        tools_box.pack_start(info_label, False, False, 0)

        # Separator
        tools_box.pack_start(Gtk.Separator(orientation=Gtk.Orientation.HORIZONTAL), False, False, 10)

        # Anleitung
        help_label = Gtk.Label()
        help_label.set_markup("<b>Anleitung</b>")
        help_label.set_alignment(0, 0.5)
        tools_box.pack_start(help_label, False, False, 0)

        help_text = Gtk.Label()
        help_text.set_markup(
            "<small>Klicke auf \"Neues Level\", um das Board zu leeren.\n"
            "Dieser Editor unterstützt NICHT das Erstellen/Laden von benutzerdefinierten farbigen Bricks.\n"
            "Um ein bestehendes Level zu laden/bearbeiten, kopiere es in die Textarea und klicke auf \"Data→Board\".\n"
            "Wenn du ein Level erstellt hast, klicke auf \"Board→Data\" und kopiere den Text in data/levels.txt.</small>"
        )
        help_text.set_line_wrap(True)
        help_text.set_alignment(0, 0)
        tools_box.pack_start(help_text, False, False, 0)

        content_paned.add2(tools_box)
        content_paned.set_position(800)

        # Standardwert für Level-Daten
        self.level_buffer.set_text("** Start **\n" + "0" * 52 * 23 + "\n** Stop **")

        # UI anzeigen
        self.window.show_all()

        # Initial Board zeichnen
        self.data_to_board(None)
        self.draw_level()

    def new_level(self, button):
        for i in range(598):
            self.bricks[i] = ["0", "0"]
        self.draw_level()

    def board_to_data(self, button):
        self.draw_level()

    def data_to_board(self, button):
        buffer_text = self.level_buffer.get_text(
            self.level_buffer.get_start_iter(),
            self.level_buffer.get_end_iter(),
            True
        )

        # Level-Daten parsen
        if "** Start **" in buffer_text and "** Stop **" in buffer_text:
            # Text zwischen Start und Stop extrahieren
            match = re.search(r'\*\* Start \*\*(.*?)\*\* Stop \*\*', buffer_text, re.DOTALL)
            if match:
                data = match.group(1).replace("\n", "")
                ch = 0

                for i in range(min(598, len(data) // 2)):
                    self.bricks[i][0] = data[ch]
                    self.bricks[i][1] = data[ch + 1]
                    ch += 2

                self.draw_level()
        else:
            dialog = Gtk.MessageDialog(
                transient_for=self.window,
                flags=0,
                message_type=Gtk.MessageType.ERROR,
                buttons=Gtk.ButtonsType.OK,
                text="Ungültiges Level-Format"
            )
            dialog.format_secondary_text(
                "Das Level-Format ist ungültig. Es fehlen die Marker '** Start **' und '** Stop **'."
            )
            dialog.run()
            dialog.destroy()

    def on_brick_type_selected(self, button, brick_id):
        self.selected_type = brick_id
        self.selected_brick_image.set_from_pixbuf(self.pixbufs.get(brick_id))
        self.selected_brick_label.set_markup(f"<b>Ausgewählter Brick-Typ:</b> {brick_id}")

    def on_powerup_changed(self, combo):
        tree_iter = combo.get_active_iter()
        if tree_iter is not None:
            model = combo.get_model()
            powerup_id = model[tree_iter][0]
            self.selected_powerup = powerup_id
            self.selected_powerup_label.set_markup(f"<b>Ausgewählter Powerup:</b> {powerup_id}")

    def on_brick_clicked(self, eventbox, event, brick_num):
        if self.bricks[brick_num][1] == self.selected_type:
            self.bricks[brick_num][0] = "0"
            self.bricks[brick_num][1] = "0"
        else:
            self.bricks[brick_num][0] = self.selected_powerup
            self.bricks[brick_num][1] = self.selected_type

        # Aktualisieren der Anzeige für diesen Brick
        image = self.level_grid.get_child_at(brick_num % self.bricks_per_row, brick_num // self.bricks_per_row)
        if image and isinstance(image, Gtk.EventBox):
            img = image.get_children()[0]
            img.set_from_pixbuf(self.pixbufs.get(self.bricks[brick_num][1]))

    def draw_level(self):
        # Level-Grid leeren
        for child in self.level_grid.get_children():
            self.level_grid.remove(child)

        # Level-Daten aufbauen
        level_data = "** Start **\n"
        col = 0

        for i, brick in enumerate(self.bricks):
            if col == self.bricks_per_row:
                level_data += "\n"
                col = 0

            # Brick zum Grid hinzufügen
            eventbox = Gtk.EventBox()
            img = Gtk.Image.new_from_pixbuf(self.pixbufs.get(brick[1]))
            eventbox.add(img)
            eventbox.connect("button-press-event", self.on_brick_clicked, i)

            self.level_grid.attach(eventbox, i % self.bricks_per_row, i // self.bricks_per_row, 1, 1)

            # Daten für Text aufbauen
            level_data += brick[0] + brick[1]
            col += 1

        level_data += "\n** Stop **"

        # Level-Daten im Textfeld aktualisieren
        self.level_buffer.set_text(level_data)

        # Alle Widgets anzeigen
        self.level_grid.show_all()


if __name__ == "__main__":
    app = SDLBallLevelEditor()
    Gtk.main()
