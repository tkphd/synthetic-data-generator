import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

df = pd.read_csv('triple-node-march.log', delimiter=r"\s+")

fig, axs = plt.subplots(nrows=1, ncols=2, sharey=True, figsize=(11, 3))
fig.suptitle("BeeGFS Writing 100 MB Random Dataset", fontsize=14)

ax = axs[0]
ax.set_xlabel('avg file size / B')
ax.set_xscale('log')
ax.set_ylabel('write time / s')
# ax.set_yscale('log')
ax.scatter(df['file'], df['avg'], color='blue', label='write')
ax.scatter(df['file'], df['total'], color='red', label='total')
ax.legend(loc='upper right', fontsize=10)

ax = axs[1]
ax.set_xlabel('num files')
ax.set_xscale('log')
ax.scatter(100000000 / df['file'], df['avg'], color='blue', label='write')
ax.scatter(100000000 / df['file'], df['total'], color='red', label='total')

fig.savefig('march-100M.png', bbox_inches='tight', dpi=400)
