import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score
import joblib

# Load data into DataFrame
data = pd.read_csv('data.csv', sep=";")

# Split data into input features (X) and target variable (y)
X = data[['Branch instructions', 'Branch misses']]
y = data['Attack']

# Split data into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3, random_state=42)

# Train a logistic regression model
model = LogisticRegression()
model.fit(X_train, y_train)

# Make predictions on test set
y_pred = model.predict(X_test)

# Evaluate performance using accuracy score
accuracy = accuracy_score(y_test, y_pred)
print('Accuracy:', accuracy)

# Save the model for deployment
joblib.dump(model, 'spectre-detector.pkl')