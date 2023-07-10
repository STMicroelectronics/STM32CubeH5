from setuptools import setup, find_packages
import re

with open("README.md", 'r') as f:
    long_description = f.read()

with open("ITSbuilder/main.py", "r") as f:
    version="unknown"
    for line in f.readlines():
        res=re.search("^\s*SOURCE_VERSION\s*=\s*\"([\d\w\.]*)\"\s*$", line)
        if res:
            version=res.group(1).lower()
            break

setup(
    name="ITSBuilder",
    version=version,
    description="Update application project files",
    long_description=long_description,
    packages=find_packages('ITSBuilder'),
    license_files =("LICENSE.md", "LICENSE.txt",),
    python_requires='>=3.10',
    install_requires=[
        'click'
    ],
    entry_points={
        "console_scripts": ["ITSBuilder=main:main"]
    }
)
