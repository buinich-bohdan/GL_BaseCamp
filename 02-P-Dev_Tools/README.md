# Lee algorithm
![N](https://image.ibb.co/kMgG2T/Screenshot_from_2018_07_07_03_31_05.png)

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


![N](https://image.ibb.co/g8b5wo/Screenshot_from_2018_07_07_03_40_04.png)

