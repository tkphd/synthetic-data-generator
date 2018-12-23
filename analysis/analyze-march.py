import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv('triple-node-march-b.log', delimiter=r"\s+")

fig, axs = plt.subplots(nrows=1, ncols=2, sharey=True, figsize=(11, 3))
fig.suptitle("BeeGFS Writing 100 MB Random Dataset", fontsize=14)

ax = axs[0]
ax.set_xlabel('avg file size / B')
ax.set_xscale('log')
ax.set_ylabel('time / s')
# ax.errorbar(df.file, df.write, yerr=[df.stdev, df.stdev], color='blue', label='create')
ax.plot(df.file, df.write, color='blue', label='create')
ax.plot(df.file, df.remove, color='red', label='remove')
ax.legend(loc='upper right', fontsize=10)

ax = axs[1]
ax.set_xlabel('num files')
ax.set_xscale('log')
ax.plot(100000000 / df.file, df.write, color='blue', label='create')
ax.plot(100000000 / df.file, df.remove, color='red', label='remove')

fig.savefig('march-100M.png', bbox_inches='tight', dpi=400)
