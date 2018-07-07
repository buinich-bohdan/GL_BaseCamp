# Lee algorithm
![N](https://lh5.googleusercontent.com/p92JgP1VVVExDLF9_ahisG8Tj8OPfc1mt5DlsNRnZzAdduYlTXd2kjjIkfCKWpiO-opNcrVxYJqXp-kvLEcX=w1366-h632)
### TechAlgorithm
1) Initialization
  - Select start point, mark with 0
  - i := 0
2) Wave expansion
  - REPEAT
      - Mark all unlabeled neighbors of points marked with i with i+1
      - i := i+1
    UNTIL ((target reached) or (no points can be marked))
3) Backtrace
    - go to the target point
    REPEAT
      - go to next node that has a lower mark than the current node
      - add this node to path
    UNTIL (start point reached)
4) Clearance
  - Block the path for future wirings
  - Delete all marks

Of course the wave expansion marks only points in the routable area of the chip, not in the blocks or already wired parts, and to minimize segmentation you should keep in one direction as long as possible.

# To generate the maze you can use the script arrayGENERATOR
**set  num_rows and num_columns**
```sh
$ arrayGENERATOR
```
### Resultant array to be inserted into the program and set the start point (0) and end point (-3)


![N](https://lh3.googleusercontent.com/CaI5xRp0qN-ByOZDJm_V1fGoTcH5_7JoQc0RqyPQGB9lAnNulms5wYE9HeAn5wAsOR6r3ag6_IOJE7X7hQiYy4Uo_bo7f4wBe9lVRnNl_Who-brMwNZymytYnR7BwJZUkxXsETHfOobQMLh9I9ceCtIhm3zTk1aWzXWc2HLEkHdKyeVLMirmKIGlxIeFh5GwpM7hNUojF0BEfXKrgZAeyHOFjqrKz7MHqDnAChe4bHPtnF649_wdeHvsVKrNZNzalE6BcXBlU4sMb6X1M6aAOfWtMzAPggwualseIe21e6yIAqC8_6Lu9tXa7vmx3E2cefoh1thZj364QF6Hn3A5ntS2TixNeIMmAOThEGG-pxtpEX6KU0EFz5C49k5tuxVfosx2o6LS1srQEhRsn_Omp9KxdWfEVqsM8UAWzbuBaaDDSLht2T2tS7PxZ0xsal3eCeJXQHY-LK7eSr95nQp6bLDGksBs7AB3QZoK9K5jCZ8kuSutHMYunKCakhz6dDzkwj89iXpIzRNPR61HtbvXzj1mBQh8r-z5ybskehgQ4JygwoBh0E7Z0J-gCHYn7upTRbL5oBSJtm_ymK4dFzylhVgPElnjohO5RzrpUA=w933-h183-no)

