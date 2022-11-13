
class Vehicle:

    color = 'white'

    def __init__(self, name, max_speed, mileage) -> None:
        self.name = name
        self.max_speed = max_speed
        self.mileage = mileage

    def __repr__(self) -> str:
        return f"Vehicle - {self.name}"

    def seating_capacity(self, capacity):
        return f"The seating capacity of a {self.name} is {capacity} passengers"
    
    def fare(self):
        return self.capacity * 100

class Bus(Vehicle):

    def __init__(self, name, max_speed, mileage) -> None:
        super().__init__(name, max_speed, mileage)
    
    def __repr__(self) -> str:
        return "BUS_"+super().__repr__() 

    def seating_capacity(self, capacity=50):
        return super().seating_capacity(capacity=50)

    def fare(self):
        return super().fare()*1.1

class Car(Vehicle):
    pass
