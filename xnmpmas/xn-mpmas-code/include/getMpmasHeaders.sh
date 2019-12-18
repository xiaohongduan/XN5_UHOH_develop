for i in agents crops landscape markets others solvers
do
	cp -r $HOME/workingCopyV3/mpmas/$i/include/* ./
done

cp -r $HOME/workingCopyV3/utilities/include/* ./
cp -r $HOME/workingCopyV3/libraries/RngStream/include/* ./
cp -r $HOME/workingCopyV3/libraries/cplexOsi128/OsiCpxStatic128 ./
