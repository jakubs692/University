package institutions;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Tests for the TicketForm class (filling out the bet slip).
 */
public class TicketFormTest {

    private final TicketForm form = new TicketForm();

    @Test
    public void testValidBetStructure() {
        // Fill all 8 fields with 6 numbers each (1-6)
        for (int i = 1; i < 9; i++) {
            for (int j = 1; j < 7; j++) {
                form.markNumber(i, j);
            }
        }

        for (int i = 1; i < 9; i++) {
            assertTrue(form.isValidBet(i), "Field " + i + " should be valid.");
        }
    }

    @Test
    public void testValidBetCount() {
        // Fill first 4 fields correctly
        for (int i = 1; i < 5; i++) {
            for (int j = 1; j < 7; j++) {
                form.markNumber(i, j);
            }
        }

        // Fill remaining fields incorrectly (too many numbers)
        for (int i = 5; i < 9; i++) {
            for (int j = 1; j < 20; j++) {
                form.markNumber(i, j);
            }
        }
        
        assertEquals(4, form.countValidBets(), "Should find exactly 4 valid bets.");
    }

    @Test
    public void testCancelledField() {
        // Fill 4 fields correctly
        for (int i = 1; i < 5; i++) {
            for (int j = 1; j < 7; j++) {
                form.markNumber(i, j);
            }
        }
        // Cancel the first field
        form.cancelField(1);
        
        assertEquals(3, form.countValidBets(), "Should find 3 valid bets after cancelling one.");
    }
}