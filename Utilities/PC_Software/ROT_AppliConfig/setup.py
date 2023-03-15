import setuptools

setuptools.setup(
    name="AppliCfg",
    description=("Update application project files"),
    packages=setuptools.find_packages(),
    python_requires='>=3.10',
    install_requires=[
        'click',
    ],
    packages=["AppliCfg"],
    entry_points={
        "console_scripts": ["AppliCfg=AppliCfg.main:AppliCfg"]
    },
)
