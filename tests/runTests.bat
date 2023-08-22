if exist testsResults.log del testsResults.log
echo =============================================>> testsResults.log
echo ===^> Run Tests At %DATE% %TIME% >> testsResults.log
echo =============================================>> testsResults.log

cd .\CoordMerge
call runCoordMergeSUMO.bat

