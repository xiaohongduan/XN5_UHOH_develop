for i in agents crops landscape markets others solvers
do
	cp -r $HOME/code/mpmas/$i/include/* ./
done

cp -r $HOME/code/utilities/include/* ./
cp -r $HOME/code/libraries/RngStream/include/* ./
cp -r $HOME/code/libraries/cplexOsi/OsiCpxStatic ./
