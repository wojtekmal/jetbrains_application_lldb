rm -rf build
zip -r archive.zip . -x archive.zip -x ".git*"
git add .
git commit -m "Updating"
git push