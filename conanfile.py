from conan import ConanFile
from conan.tools.files import copy
from conan.tools.build import check_min_cppstd
from conan.tools.microsoft import MSBuild, vs_layout, MSBuildDeps, MSBuildToolchain
import os

class SQLiteWrapperConan(ConanFile):
    # 1. BASIC INFO
    name = "sqlite-wrapper"
    version = "0.1.0"

    # 2. BUILD SETTINGS
    settings = "os", "compiler", "build_type", "arch"

    options = {
        "with_profiling": [True, False],
    }
    # Set the default value
    default_options = {
        "with_profiling": False,
    }

    def validate(self):
        # This will fail the "conan create" if compiler.cppstd is not 17 or higher
        check_min_cppstd(self, "17")

    # 3. REQUIREMENTS
    # This tells Conan that "mydependentlib" requires "mypubliclib"
    # to build and to be consumed.
    def requirements(self):
        self.requires("o2profiler/0.1.0")

    # 3. GENERATORS (for consumers)
    #generators = "MSBuildDeps", "MSBuildToolchain"

    # 4. SOURCE CODE
    # This tells conan create to copy our project files
    # into its build environment.
    exports_sources = (
        "SQLiteWrapper.sln",
        "SQLiteWrapper/SQLiteWrapper.vcxproj",
        "SQLiteWrapper/include/*",
        "SQLiteWrapper/src/*",
        "Directory.Build.props",
        "Directory.Build.targets"
    )

    # 5. LAYOUT
    # This tells Conan to expect the Visual Studio layout
    # (e.g., .sln in root, .vcxproj in subfolder)
    def layout(self):
        vs_layout(self)

    # 7. GENERATE (for this project's build)
    # This generates the .props files for THIS build
    def generate(self):
        deps = MSBuildDeps(self)
        deps.generate()

        tc = MSBuildToolchain(self)
        # If the "with_profiling" option is true, add a define
        if self.options.with_profiling:
            tc.preprocessor_definitions["ENABLE_PROFILING"] = "1"
        tc.generate()

    # 6. BUILD METHOD
    # This tells Conan how to build your code
    def build(self):
        msbuild = MSBuild(self)
        # Build the .vcxproj file directly, not the .sln
        project_file = os.path.join(
            self.source_folder, 
            "SQLiteWrapper", 
            "SQLiteWrapper.vcxproj"
        )
        msbuild.build(project_file)

    # 7. PACKAGE METHOD
    # This is the most important part.
    # It copies build files into the final package.
    def package(self):
        # --- THE HEADER FIX ---
        # Copy all .h files FROM the project subfolder...
        copy(
            self, "*",
            src=os.path.join(self.source_folder, "SQLiteWrapper", "include"),
            # ...TO the "include" folder in the package
            dst=os.path.join(self.package_folder, "include")
        )

        # --- THE LIBRARY FILE ---
        # Copy the .lib file
        copy(
            self, "*.lib",
            src=self.build_folder,
            dst=os.path.join(self.package_folder, "lib"),
            keep_path=False
        )

    # 8. PACKAGE INFO (for consumers)
    # This tells consumers what to link against.
    def package_info(self):
        # Default to the release name
        lib_name = "SQLiteWrapper"

        # If the consumer is in Debug, change the name
        if self.settings.build_type == "Debug":
            lib_name += "-d"

        # Tell the consumer to link that specific library
        self.cpp_info.libs = [lib_name]