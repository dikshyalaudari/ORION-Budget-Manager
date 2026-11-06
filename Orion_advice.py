import re
import os
import sys

def extract_budget_info(filename):
    try:
        with open(filename, "r") as f:
            lines = f.readlines()

        # Start reading only after --- Monthly Budget --- line
        start = next(i for i, line in enumerate(lines) if "--- Monthly Budget ---" in line)
        budget_lines = "".join(lines[start:])

        income = float(re.search(r"Total Budget:\s*\$?(\d+(?:\.\d+)?)", budget_lines).group(1))
        expenses = float(re.search(r"Total Expenses:\s*\$?(\d+(?:\.\d+)?)", budget_lines).group(1))
        return income, expenses
    except Exception as e:
        print("❌ Failed to parse data from file. Ensure formatting matches expected structure.")
        return None, None

def generate_suggestions(income, expenses):
    suggestions = []
    difference = expenses - income
    if difference <= 0:
        return ["🎉 You're within budget! Keep it up! 💪"]

    proportions = {
        "Others": 0.20, "Entertainment": 0.15,
        "Groceries": 0.15, "Education": 0.15,
        "Insurance": 0.10, "Rent": 0.25
    }

    for category, percent in proportions.items():
        cut = round(difference * percent, 2)
        suggestions.append(f"💡 Reduce {category} spending by ~${cut}")

    return suggestions

def main():
    try:
        with open(".current_user", "r") as f:
            userfile = f.read().strip()
    except FileNotFoundError:
        print("⚠️ .current_user not found. Please log in via Orion C app first.")
        return

    if not os.path.exists(userfile):
        print(f"❌ Budget file '{userfile}' does not exist.")
        return

    print(f"\n🤖 Orion AI Assistant - Analysis for '{userfile}'")

    income, expenses = extract_budget_info(userfile)
    if income is None or expenses is None:
        return

    print(f"\n📊 Summary:\n  Income: ${income:.2f}\n  Expenses: ${expenses:.2f}")
    if expenses > income:
        print(f"⚠️ Overspending by ${expenses - income:.2f}")
        print("\n🔧 Suggestions:")
        for s in generate_suggestions(income, expenses):
            print("  -", s)
        print("\n🌟 Tip: Review your top 3 expense categories first.")
    else:
        print("✅ You're on track! Consider boosting savings this month.")

if __name__ == "__main__":
    main()
