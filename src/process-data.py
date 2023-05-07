import pandas as pd

df = pd.read_csv('./output.csv', sep=';')
group = df.groupby('Index').describe().to_csv('./output-pandas.csv', sep=';');