import pandas as pd

df = pd.read_csv('./output.csv', sep=';')
index = df['Index']
missrate = df['Branch miss rate']

group = df.groupby('Index').describe().to_csv('./output-pandas.csv', sep=';');