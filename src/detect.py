import pandas as pd
from sklearn.preprocessing import StandardScaler
import joblib
from sklearn.metrics import accuracy_score

# Load new data from CSV file
new_data = pd.read_csv('data.csv', sep=';')

# Extract input features and preprocess them
X_new = new_data[['Branch instructions', 'Branch misses']]
scaler = StandardScaler()
X_new = scaler.fit_transform(X_new)

# Load trained model from file
model = joblib.load('spectre-detector.pkl')

# Make predictions on new data
y_pred = model.predict(X_new)

# Print predicted output values
print('Predicted output values:')
print(y_pred)

# Evaluate model performance (if true values are available)
y_true = new_data['Attack']
accuracy = accuracy_score(y_true, y_pred)
print('Accuracy:', accuracy)