from __future__ import annotations

import os
import platform
import shutil
import subprocess
import tempfile
from pathlib import Path

from setuptools import find_packages, setup
from setuptools.command.build_py import build_py


class BuildPy(build_py):
    def run(self) -> None:
        self._build_native_library()
        super().run()

    def _build_native_library(self) -> None:
        repo_root = Path(__file__).resolve().parent
        build_type = os.environ.get("VIBRA_CMAKE_BUILD_TYPE", "Release")

        with tempfile.TemporaryDirectory(prefix="vibra-cmake-build-") as build:
            build_dir = Path(build)
            configure_cmd = [
                "cmake",
                "-B",
                str(build_dir),
                "-S",
                str(repo_root),
                "-DLIBRARY_ONLY=ON",
                "-DBUILD_TESTING=OFF",
                f"-DCMAKE_BUILD_TYPE={build_type}",
            ]
            build_cmd = [
                "cmake",
                "--build",
                str(build_dir),
                "--target",
                "vibra_shared",
                "--config",
                build_type,
            ]

            subprocess.check_call(configure_cmd)
            subprocess.check_call(build_cmd)

            library = self._find_library(build_dir)
            target_dir = Path(self.build_lib) / "vibra" / "native"
            target_dir.mkdir(parents=True, exist_ok=True)
            shutil.copy2(library, target_dir / library.name)

    @staticmethod
    def _find_library(build_dir: Path) -> Path:
        system = platform.system()
        if system == "Darwin":
            patterns = ["libvibra*.dylib"]
        elif system == "Windows":
            patterns = ["vibra*.dll", "libvibra*.dll"]
        else:
            patterns = ["libvibra*.so"]

        for pattern in patterns:
            matches = sorted(build_dir.rglob(pattern))
            if matches:
                return matches[0]

        raise RuntimeError(f"Could not find built libvibra shared library in {build_dir}")


setup(
    name="vibra",
    version="0.1.0",
    description="Python bindings for vibra music fingerprint generation",
    long_description=(Path(__file__).parent / "python" / "README.md").read_text(),
    long_description_content_type="text/markdown",
    license="GPL-3.0-or-later",
    python_requires=">=3.8",
    install_requires=["requests>=2.31"],
    package_dir={"": "python/src"},
    packages=find_packages("python/src"),
    package_data={"vibra": ["py.typed", "native/*"]},
    zip_safe=False,
    cmdclass={"build_py": BuildPy},
)
