import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, Circle, FancyArrowPatch

# ==========================================
# ROBOT CONFIGURATION
# ==========================================
L1 = 15.0  # cm
L2 = 15.0  # cm

# LAYOUT DIMENSIONS
MACHINE_W = 40.0   # Width of table
MACHINE_H = 40.0   # Height of table
GAME_SIZE = 32.0   # 32x32cm Chess Grid
SQUARE_SIZE = GAME_SIZE / 8.0

# SHIFT THE BOARD LEFT (Negative X)
# This creates empty space on the RIGHT side
BOARD_OFFSET_X = -3.0 
BOARD_OFFSET_Y = 0.0

# Calculate the resulting Graveyard Width
# Distance from Right Edge of Board to Right Edge of Machine
board_right_edge = BOARD_OFFSET_X + (GAME_SIZE / 2)
machine_right_edge = MACHINE_W / 2
GRAVEYARD_WIDTH = machine_right_edge - board_right_edge

# ==========================================
# INVERSE KINEMATICS SOLVER
# ==========================================
def solve_ik(x, y, l1, l2):
    r_sq = x**2 + y**2
    r = np.sqrt(r_sq)
    
    if r > (l1 + l2) or r == 0:
        return None, None, False

    c2 = (r_sq - l1**2 - l2**2) / (2 * l1 * l2)
    if c2 > 1.0: c2 = 1.0
    if c2 < -1.0: c2 = -1.0
        
    theta2 = np.arccos(c2)
    k1 = l1 + l2 * c2
    k2 = l2 * np.sin(theta2)
    theta1 = np.arctan2(y, x) - np.arctan2(k2, k1)
    
    return theta1, theta2, True

# ==========================================
# VISUALIZATION
# ==========================================
fig, ax = plt.subplots(figsize=(10, 8))
plt.subplots_adjust(bottom=0.05)
ax.set_xlim(-25, 25)
ax.set_ylim(-25, 25)
ax.set_aspect('equal')
ax.axis('off') # Cleaner look
ax.set_title(f"Final Layout: Side Shift\nGraveyard Width: {GRAVEYARD_WIDTH:.1f} cm", fontsize=14)

# --- 1. Draw Machine Boundary ---
table = Rectangle((-MACHINE_W/2, -MACHINE_H/2), 
                  MACHINE_W, MACHINE_H, 
                  linewidth=2, edgecolor='#555', facecolor='#f9f9f9', 
                  label='Table')
ax.add_patch(table)

# --- 2. Draw Graveyard / Control Panel (Right Side) ---
# Starts at the right edge of the chess board
gy_x = BOARD_OFFSET_X + GAME_SIZE/2
gy_y = -MACHINE_H/2
gy_w = GRAVEYARD_WIDTH
gy_h = MACHINE_H

graveyard = Rectangle((gy_x, gy_y), gy_w, gy_h, 
                      facecolor='#e1f5fe', edgecolor='none')
ax.add_patch(graveyard)
ax.text(gy_x + gy_w/2, 0, "CONTROLS\n&\nGRAVEYARD", 
        ha='center', va='center', color='#0277bd', fontweight='bold', rotation=0)

# --- 3. Draw Chess Board ---
start_x = -GAME_SIZE/2 + BOARD_OFFSET_X
start_y = -GAME_SIZE/2 + BOARD_OFFSET_Y

for row in range(8):
    for col in range(8):
        x_pos = start_x + col * SQUARE_SIZE
        y_pos = start_y + row * SQUARE_SIZE
        color = '#f0d9b5' if (row + col) % 2 == 0 else '#b58863' 
        sq = Rectangle((x_pos, y_pos), SQUARE_SIZE, SQUARE_SIZE, facecolor=color)
        ax.add_patch(sq)

# Board Outline
board_outline = Rectangle((start_x, start_y), GAME_SIZE, GAME_SIZE, 
                          linewidth=2, edgecolor='#3e2723', facecolor='none')
ax.add_patch(board_outline)

# --- 4. DIMENSION LINES (The Measurements) ---
def draw_dim(x1, y1, x2, y2, text, offset=2):
    ax.annotate("", xy=(x1, y1), xytext=(x2, y2),
                arrowprops=dict(arrowstyle='<->', color='black', lw=1))
    mid_x = (x1 + x2) / 2
    mid_y = (y1 + y2) / 2
    ax.text(mid_x, mid_y, text, ha='center', va='bottom', 
            fontsize=9, backgroundcolor='white', fontweight='bold')

# Show Graveyard Width (Bottom Right)
draw_dim(gy_x, -MACHINE_H/2 - 2, gy_x + gy_w, -MACHINE_H/2 - 2, f"{GRAVEYARD_WIDTH}cm")

# Show Board Width (Bottom Left)
draw_dim(start_x, -MACHINE_H/2 - 2, start_x + GAME_SIZE, -MACHINE_H/2 - 2, f"{GAME_SIZE}cm")

# Show Total Width (Top)
draw_dim(-MACHINE_W/2, MACHINE_H/2 + 2, MACHINE_W/2, MACHINE_H/2 + 2, f"Total: {MACHINE_W}cm")

# --- 5. Robot Visuals ---
base_circle = Circle((0, 0), 1.0, color='black', zorder=10)
ax.add_patch(base_circle)
ax.text(0, -1.8, "(0,0)", ha='center', fontsize=8)

link1, = ax.plot([], [], 'o-', lw=5, color='#333333', solid_capstyle='round')
link2, = ax.plot([], [], 'o-', lw=5, color='#ff7f0e', solid_capstyle='round')

def update(event):
    if event.inaxes != ax: return
    mx, my = event.xdata, event.ydata
    
    t1, t2, reachable = solve_ik(mx, my, L1, L2)
    
    if reachable:
        x1 = L1 * np.cos(t1)
        y1 = L1 * np.sin(t1)
        x2 = x1 + L2 * np.cos(t1 + t2)
        y2 = y1 + L2 * np.sin(t1 + t2)
        
        link1.set_data([0, x1], [0, y1])
        link2.set_data([x1, x2], [y1, y2])
        
        # Check boundary
        if abs(mx) > MACHINE_W/2 or abs(my) > MACHINE_H/2:
             link2.set_color('red') 
        else:
             link2.set_color('#ff7f0e')

    fig.canvas.draw_idle()

fig.canvas.mpl_connect('motion_notify_event', update)
plt.show()