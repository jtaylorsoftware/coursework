import SudokuBoard
import Variable
import Domain
import Trail
import Constraint
import ConstraintNetwork
import time

class BTSolver:

    # ==================================================================
    # Constructors
    # ==================================================================

    def __init__ ( self, gb, trail, val_sh, var_sh, cc ):
        self.network = ConstraintNetwork.ConstraintNetwork(gb)
        self.hassolution = False
        self.gameboard = gb
        self.trail = trail

        self.varHeuristics = var_sh
        self.valHeuristics = val_sh
        self.cChecks = cc

    # ==================================================================
    # Consistency Checks
    # ==================================================================

    # Basic consistency check, no propagation done
    def assignmentsCheck ( self ):
        for c in self.network.getConstraints():
            if not c.isConsistent():
                return False
        return True

    """
        Part 1 TODO: Implement the Forward Checking Heuristic

        This function will do both Constraint Propagation and check
        the consistency of the network

        (1) If a variable is assigned then eliminate that value from
            the square's neighbors.

        Note: remember to trail.push variables before you change their domain
        Return: true is assignment is consistent, false otherwise
    """
    def forwardChecking ( self ):
        # all variables that are in modified constraints
        modified_vars = []
        # all neighbors of variables in modified constraints
        var_neighbors = []
        # all assignments of variables in modified constraints
        var_assignments = []

        # Gather all constraints with modified vars
        for constraint in self.network.getModifiedConstraints():
            modified_vars.extend(constraint.vars)



        # Gather all neighbors of the (potentially) modified vars
        for var in modified_vars:
            neighbors = self.network.getNeighborsOfVariable(var)
            var_neighbors.extend(neighbors)
            # the var may have an assignment of 0 (unassigned), which is
            # a roundabout way to only eliminate assigned values without
            # using an if statement
            assignments = [var.getAssignment()] * len(neighbors)
            var_assignments.extend(zip(neighbors,assignments))

        # Remove assigned values from the domains of neighbors
        for neighbor,assignment in var_assignments:
            # check if the neighbor's domain has the assignment to determine if 
            # a trail push is needed (push after a removal would be wrong) 
            if neighbor.getDomain().contains(assignment):
                self.trail.push(neighbor)
                neighbor.removeValueFromDomain(assignment)
                if neighbor.size() == 0:
                    # No remaining values, so the assignment is inconsistent.
                    return False
                    
        return True # all variables affected by recent assignments have non-zero size domains

    """
        Part 2 TODO: Implement both of Norvig's Heuristics

        This function will do both Constraint Propagation and check
        the consistency of the network

        (1) If a variable is assigned then eliminate that value from
            the square's neighbors.

        (2) If a constraint has only one possible place for a value
            then put the value there.

        Note: remember to trail.push variables before you change their domain
        Return: true is assignment is consistent, false otherwise
    """
    def norvigCheck ( self ):
        # (1) use forward checking to eliminate values from assigned variables' neighbors
        if not self.forwardChecking():
            return False

        constraints = self.network.getConstraints()
        N = self.gameboard.N
        
        # (2) check every constraint to see if a value has only one valid assignment
        for constraint in constraints:
            # dict where keys are Sudoku Values and values are the Variables that have
            # that Value in its Domain
            val_var_dict = { val: [] for val in range(1,N+1)}

            # list of counters of occurrence of a value in each constraint
            counter = [0] * N

            # count occurrences of each value in the constraint
            for i in range(N): 
                for value in constraint.vars[i].getDomain().values:
                    counter[value-1] += 1
                    val_var_dict[value].append(constraint.vars[i]) # keep track of the variable that had this value

            # for each value that has one possible location, assign the value
            # to that location and propagate the assignment    
            for i in range(N):
                if counter[i] == 1:
                    value = i+1
                    var = val_var_dict[value][0]

                    if not var.isAssigned():
                        # the variable may have the value in its domain because
                        # it's actually already assigned, so don't do anything
                        # if this is the case
                        self.trail.push(var)
                        var.assignValue(value)

                        # Propagate the assignment to the variable's neighbors.
                        # I.e., if this constraint is a row, then variables in the
                        # row won't have the value, but variables in the column
                        # or block may have it, so it needs to be removed from there.
                        for neighbor in self.network.getNeighborsOfVariable(var):
                            # don't do propagation if the neighbor is assigned already
                            if neighbor.isAssigned():
                                continue 

                            # check neighbor's domain to see if it has the assignment as a value
                            for neighbor_value in neighbor.getValues():
                                if neighbor_value == value:
                                    self.trail.push(neighbor)
                                    neighbor.removeValueFromDomain(value)
                                    if neighbor.size() == 0:
                                        # if eliminating the value results in no possible values, 
                                        # then the assignment is not valid
                                        return False 

        return True
            
                

    """
         Optional TODO: Implement your own advanced Constraint Propagation

         Completing the three tourn heuristic will automatically enter
         your program into a tournament.
     """
    def getTournCC ( self ):
        return None

    # ==================================================================
    # Variable Selectors
    # ==================================================================

    # Basic variable selector, returns first unassigned variable
    def getfirstUnassignedVariable ( self ):
        for v in self.network.variables:
            if not v.isAssigned():
                return v

        # Everything is assigned
        return None

    """
        Part 1 TODO: Implement the Minimum Remaining Value Heuristic

        Return: The unassigned variable with the smallest domain
    """
    def getMRV ( self ):
        # list of (variable, domain size) pairs for all unassigned variables in the board
        unassigned_vars = []
        # the smallest domain size for all unassigned vars
        min_domain_size = self.gameboard.N + 1
        # populate the list of unassigned vars while finding the min domain size
        for var in self.network.getVariables():
            if not var.isAssigned():
                domain_size = var.size()
                unassigned_vars.append((var, domain_size))
                if domain_size < min_domain_size:
                    min_domain_size = domain_size

        if unassigned_vars:
            for var,size in unassigned_vars:
                if size == min_domain_size:
                    return var # return first unassigned var with min domain size

        return None

    """
        Part 2 TODO: Implement the Minimum Remaining Value Heuristic
                       with Degree Heuristic as a Tie Breaker

        Return: The unassigned variable with, first, the smallest domain
                and, second, the most unassigned neighbors
    """
    def MRVwithTieBreaker ( self ):
        # list of (variable, domain size) pairs for all unassigned variables in the board
        unassigned_vars = []
        # the smallest domain size for all unassigned vars
        min_domain_size = self.gameboard.N + 1
        # populate the list of unassigned vars while finding the min domain size
        for var in self.network.getVariables():
            if not var.isAssigned():
                domain_size = var.size()
                unassigned_vars.append((var, domain_size))
                if domain_size < min_domain_size:
                    min_domain_size = domain_size
            
        if unassigned_vars:
            # list of [var, degree] for all variables whose size are the minimal domain size
            min_domain_vars = [[var, 0] for var,size in unassigned_vars if size == min_domain_size]
            if len(min_domain_vars) > 1:
                # count the number of unassigned neighbors (degree) for each variable to tie break
                for var in min_domain_vars:
                    for neighbor in self.network.getNeighborsOfVariable(var[0]):
                        if not neighbor.isAssigned():
                            var[1] += 1
                min_domain_vars = sorted(min_domain_vars,key=lambda t: -t[1])
            return min_domain_vars[0][0]
            
        return None

    """
         Optional TODO: Implement your own advanced Variable Heuristic

         Completing the three tourn heuristic will automatically enter
         your program into a tournament.
     """
    def getTournVar ( self ):
        return None

    # ==================================================================
    # Value Selectors
    # ==================================================================

    # Default Value Ordering
    def getValuesInOrder ( self, v ):
        values = v.domain.values
        return sorted( values )

    """
        Part 1 TODO: Implement the Least Constraining Value Heuristic

        The Least constraining value is the one that will knock the least
        values out of it's neighbors domain.

        Return: A list of v's domain sorted by the LCV heuristic
                The LCV is first and the MCV is last
    """
    def getValuesLCVOrder ( self, v ):
        # all possible values for v
        v_values = v.getValues()

        # can skip checking neighbors if v has only one value possible
        if len(v_values) == 1:
            return [v_values[0]]

        # contains count of each neighbor domain value
        value_count = { value: 0 for value in v_values}
        
        neighbors = self.network.getNeighborsOfVariable(v)
        neighbor_values = []

        # get all neighbor values
        for neighbor in neighbors:
            neighbor_values.extend(neighbor.getValues())

        # count occurrences of neighbor values that are also valid values for v 
        for value in neighbor_values:
            if value in value_count:
                value_count[value] += 1

        # sort the dict by the count of each value, and return the list of keys
        sorted_counts = sorted(value_count, key=value_count.get)
        return sorted_counts
            

    """
         Optional TODO: Implement your own advanced Value Heuristic

         Completing the three tourn heuristic will automatically enter
         your program into a tournament.
     """
    def getTournVal ( self, v ):
        return None

    # ==================================================================
    # Engine Functions
    # ==================================================================

    def solve ( self ):
        if self.hassolution:
            return

        # Variable Selection
        v = self.selectNextVariable()

        # check if the assigment is complete
        if ( v == None ):
            for var in self.network.variables:

                # If all variables haven't been assigned
                if not var.isAssigned():
                    print ( "Error" )

            # Success
            self.hassolution = True
            return

        # Attempt to assign a value
        for i in self.getNextValues( v ):

            # Store place in trail and push variable's state on trail
            self.trail.placeTrailMarker()
            self.trail.push( v )

            # Assign the value
            v.assignValue( i )

            # Propagate constraints, check consistency, recurse
            if self.checkConsistency():
                self.solve()

            # If this assignment succeeded, return
            if self.hassolution:
                return

            # Otherwise backtrack
            self.trail.undo()

    def checkConsistency ( self ):
        if self.cChecks == "forwardChecking":
            return self.forwardChecking()

        if self.cChecks == "norvigCheck":
            return self.norvigCheck()

        if self.cChecks == "tournCC":
            return self.getTournCC()

        else:
            return self.assignmentsCheck()

    def selectNextVariable ( self ):
        if self.varHeuristics == "MinimumRemainingValue":
            return self.getMRV()

        if self.varHeuristics == "MRVwithTieBreaker":
            return self.MRVwithTieBreaker()

        if self.varHeuristics == "tournVar":
            return self.getTournVar()

        else:
            return self.getfirstUnassignedVariable()

    def getNextValues ( self, v ):
        if self.valHeuristics == "LeastConstrainingValue":
            return self.getValuesLCVOrder( v )

        if self.valHeuristics == "tournVal":
            return self.getTournVal( v )

        else:
            return self.getValuesInOrder( v )

    def getSolution ( self ):
        return self.network.toSudokuBoard(self.gameboard.p, self.gameboard.q)
