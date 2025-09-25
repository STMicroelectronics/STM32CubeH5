#!qshel

# Run this script using ". runme.sh" to show demo commands in the shell

echo "Welcome to the Qoraal Shell Demo!"
echo "This script demonstrates some of the shell commands and their usage."
sleep 1000

echo
echo "Step 1: Stopping the system service..."
echo "Executing: ctrl system stop"
ctrl system stop
echo
sleep 1000

echo
echo "Step 2: Listing all available commands..."
echo "Executing: ?"
?
echo
sleep 2000

echo
echo "Step 3: Starting the demo service..."
echo "Executing: ctrl demo start"
ctrl demo start
echo
sleep 2000

echo
echo "Step 4: Listing commands installed by the demo service..."
echo "Executing: ? demo"
? demo
echo
sleep 2000

echo
echo "Step 5: Running the 'demo_tasks' command..."
echo "Executing: demo_tasks"
demo_tasks
echo
sleep 2000

echo
echo "Demo completed! Feel free to explore other commands or rerun the script."
:clearerror
