import pandas as pd
from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
import joblib

# Load the data from the first file
df_normal = pd.read_csv("output-measure-normal2.csv", sep=";")

# Load the data from the second file
df_attack = pd.read_csv("output-measure-attack2.csv", sep=";")

# Add a target column to both dataframes
df_normal['is attack'] = 0  # 0 for normal data
df_attack['is attack'] = 1  # 1 for attack data

# Concatenate the two dataframes
df_combined = pd.concat([df_normal, df_attack])

# Split the data into training and testing sets
X = df_combined[['cache miss rate', 'branch miss rate']]
y = df_combined['is attack']
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Train a logistic regression model
model = LogisticRegression()
model.fit(X_train, y_train)

# Make predictions on the test set
y_pred = model.predict(X_test)

# Calculate the accuracy of the model
accuracy = accuracy_score(y_test, y_pred)
print("Accuracy:", accuracy)

# Export the trained model
joblib.dump(model, "logistic_regression_model.joblib")