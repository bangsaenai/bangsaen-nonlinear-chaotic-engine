from setuptools import setup, Extension

module = Extension(
    'core.libchaotic_kernel',
    sources=['src/chaotic_core.c'],
    libraries=['user32', 'advapi32']
)

setup(
    name='libchaotic_kernel',
    version='1.0',
    description='Bangsaen Anti-AI Chaotic Engine',
    ext_modules=[module]
)