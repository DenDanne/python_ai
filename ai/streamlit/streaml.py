import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
import streamlit as st
import pandas as pd
import numpy as np

# ========================================================
# Loading Data and Modelling it.
# ========================================================

# Put your own path for reading the Excel file.
modelling_data = pd.read_excel(r'modelling_data.xlsx')

x = np.array(modelling_data["x"])
y = np.array(modelling_data["y"])

# Plotting the data.
fig_data, ax_data = plt.subplots(figsize=(8, 4))
ax_data.set_title('Data')
ax_data.scatter(modelling_data["x"], modelling_data["y"])
ax_data.set_xlabel("x")
ax_data.set_ylabel("y")

# Reshape
x = x.reshape(-1, 1)
y = y.reshape(-1, 1)

x.shape
y.shape

# Initializing and fitting our Linear Regression model.
print("test")
lin_reg = LinearRegression()
print("Fitting . . . ")
lin_reg.fit(x, y)
print("Done fitting")

# Creating new data that we will predict with our fitted model.
x_new = np.linspace(0, 2, 20).reshape(-1, 1)
y_pred_lr = lin_reg.predict(x_new)

# Plotting the data.
fig_data, ax_data = plt.subplots(figsize=(8, 4))
ax_data.set_title('Data')
ax_data.scatter(modelling_data["x"], modelling_data["y"])
ax_data.set_xlabel("x")
ax_data.set_ylabel("y")

# Display the first plot
st.write("### Original Data")
st.pyplot(fig_data)

# Initializing and fitting our Linear Regression model.
lin_reg = LinearRegression()
lin_reg.fit(x, y)

# Creating new data that we will predict with our fitted model.
x_new = np.linspace(0, 2, 20).reshape(-1, 1)
y_pred_lr = lin_reg.predict(x_new)

# Plotting the data and our model predictions.
fig_model, ax_model = plt.subplots()
ax_model.set_title("Linear Regression Model")
ax_model.scatter(x, y, label="Data")
ax_model.plot(x_new, y_pred_lr, 'r', label="Predictions")
ax_model.set_xlabel("x")
ax_model.set_ylabel("y")
ax_model.legend()

# Display the second plot
st.write("### Linear Regression Model")
st.pyplot(fig_model)


print("Finish ")