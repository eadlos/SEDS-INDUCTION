import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

data = pd.read_csv("Depth_Data.csv")

depth = pd.to_numeric(data["Depth (m)"], errors="coerce")
time = np.arange(len(depth))

print(f"Loaded {len(depth)} readings")
print(f"Found {depth.isna().sum()} unreadable value(s) in the raw file")

depth_clean = depth.copy()

jump = depth_clean.diff().abs()
spike_limit = 500
spikes = jump > spike_limit
depth_clean[spikes] = np.nan

print(f"Found {spikes.sum()} sudden spike(s) that look like sensor glitches")

depth_clean = depth_clean.interpolate()

window = 5
depth_smooth = depth_clean.rolling(window=window, center=True, min_periods=1).mean()

fig, ax = plt.subplots(figsize=(10, 6))

raw_line, = ax.plot([], [], color="lightgray", linewidth=1, label="Raw (cleaned) reading")
smooth_line, = ax.plot([], [], color="royalblue", linewidth=2, label="Smoothed depth")
current_point, = ax.plot([], [], "ro", markersize=6, label="Current reading")

ax.set_xlim(0, len(time))
ax.set_ylim(depth_clean.min() - 50, depth_clean.max() + 50)
ax.set_xlabel("Time (s)")
ax.set_ylabel("Depth (m)")
ax.set_title("Athena's Intern - Live Depth Feed")
ax.grid(True, linestyle="--", alpha=0.5)
ax.legend(loc="lower left")

depth_text = ax.text(0.02, 0.95, "", transform=ax.transAxes, fontsize=11,
                      verticalalignment="top",
                      bbox=dict(boxstyle="round", facecolor="white", alpha=0.8))


def update(frame):
    x = time[:frame + 1]
    raw_line.set_data(x, depth_clean[:frame + 1])
    smooth_line.set_data(x, depth_smooth[:frame + 1])
    current_point.set_data([time[frame]], [depth_smooth.iloc[frame]])
    depth_text.set_text(f"t = {time[frame]}s   depth = {depth_smooth.iloc[frame]:.1f} m")
    return raw_line, smooth_line, current_point, depth_text


ani = animation.FuncAnimation(fig, update, frames=len(time),
                               interval=1000, blit=True, repeat=False)

plt.tight_layout()
plt.show()

# writer = animation.PillowWriter(fps=1)
# ani.save("depth_animation.gif", writer=writer)
