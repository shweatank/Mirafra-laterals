"""
Example Test Cases
These demonstrate different test scenarios for the CI/CD pipeline
"""

import pytest


class TestBasicFunctionality:
    """Basic functionality tests"""
    
    def test_sample_pass(self):
        """This test should pass"""
        assert 1 + 1 == 2
    
    def test_string_operation(self):
        """Test string operations"""
        text = "hello world"
        assert text.upper() == "HELLO WORLD"
    
    def test_list_operations(self):
        """Test list manipulations"""
        numbers = [1, 2, 3, 4, 5]
        assert sum(numbers) == 15
        assert len(numbers) == 5


class TestMathOperations:
    """Math operation tests"""
    
    def test_addition(self):
        """Test addition"""
        assert 5 + 3 == 8
    
    def test_multiplication(self):
        """Test multiplication"""
        assert 4 * 5 == 20
    
    @pytest.mark.skip(reason="Demonstrating skipped test")
    def test_skipped_test(self):
        """This test is skipped"""
        assert False


class TestErrorHandling:
    """Error handling tests"""
    
    def test_division_by_zero(self):
        """Test that division by zero raises error"""
        with pytest.raises(ZeroDivisionError):
            result = 10 / 0
    
    def test_type_error(self):
        """Test that type errors are caught"""
        with pytest.raises(TypeError):
            result = "string" + 5


class TestDataValidation:
    """Data validation tests"""
    
    def test_email_format(self):
        """Test email validation"""
        email = "test@example.com"
        assert "@" in email
        assert "." in email
    
    def test_password_strength(self):
        """Test password validation"""
        password = "SecurePass123!"
        assert len(password) >= 8
        assert any(c.isupper() for c in password)
        assert any(c.isdigit() for c in password)


@pytest.fixture
def sample_data():
    """Fixture providing sample data"""
    return {
        "id": 1,
        "name": "Test",
        "status": "active"
    }


class TestFixtures:
    """Tests using fixtures"""
    
    def test_with_fixture(self, sample_data):
        """Test using fixture"""
        assert sample_data["id"] == 1
        assert sample_data["status"] == "active"
    
    def test_fixture_data_type(self, sample_data):
        """Test fixture data type"""
        assert isinstance(sample_data, dict)


# Parametrized tests
@pytest.mark.parametrize("input_val,expected", [
    (2, 4),
    (3, 9),
    (4, 16),
    (5, 25),
])
def test_square(input_val, expected):
    """Test square calculation with multiple inputs"""
    assert input_val ** 2 == expected


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
