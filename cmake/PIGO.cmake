include(FetchContent)

FetchContent_Declare(
        PIGO
        GIT_REPOSITORY https://github.com/GT-TDAlab/PIGO.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(PIGO)
