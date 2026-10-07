import csv
import matplotlib.pyplot as plt

epochs = []
losses = []

with open(".\\level2\\NeuralNetworks\\loss.csv", newline="") as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        epochs.append(int(row["epoch"]))
        losses.append(float(row["loss"]))

plt.plot(epochs, losses)
plt.xlabel("Epoch")
plt.ylabel("Loss")
plt.title("Training Loss")
plt.grid(True)
plt.savefig("loss_curve.png")
plt.show()