import matplotlib.pyplot as plt
import pandas as pd
import sys

if len(sys.argv) != 2:
    print("Usage: {0} record.log".format(sys.argv[0]))
else:
    f_in = str(sys.argv[1])
    f_out = f_in.replace('log', 'png')

    df = pd.read_csv(f_in, delimiter=r"\s+")
    d_size = df.data.max() / 1000000

    fig, axs = plt.subplots(nrows=1, ncols=2, figsize=(11, 3))
    fig.suptitle("BeeGFS {0} MB Write Performance".format(d_size), fontsize=14)

    ax = axs[0]
    ax.set_xlabel('avg file size / [B]')
    ax.set_xscale('log')
    ax.set_ylabel('time / [s]')
    # ax.errorbar(df.file, df.write, yerr=[df.stdev, df.stdev], color='blue', label='create')
    ax.plot(df.file, df.write, color='blue')

    ax = axs[1]
    ax.set_xlabel('avg file size / [B]')
    ax.set_xscale('log')
    ax.set_ylabel('rate / [MB/s]')
    ax.plot(df.file, df.data / (1e6 * df.write), color='green')

    fig.savefig(f_out, bbox_inches='tight', dpi=400)
