#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CANVAS_WIDTH 80
#define CANVAS_HEIGHT 24
#define MAX_OBJECTS 50

// Shape types
typedef enum {
    CIRCLE,
    RECTANGLE,
    LINE,
    TRIANGLE
} ShapeType;

// Structure to represent a shape
typedef struct {
    int id;
    ShapeType type;
    int x1, y1;           // Top-left or start point
    int x2, y2;           // Bottom-right or end point
    char fillChar;        // '*' or '_'
} Shape;

// Canvas structure
typedef struct {
    char canvas[CANVAS_HEIGHT][CANVAS_WIDTH];
    Shape objects[MAX_OBJECTS];
    int objectCount;
    int nextId;
} Canvas;

// Function declarations
Canvas* initCanvas();
void clearCanvas(Canvas *canvas);
void drawPixel(Canvas *canvas, int x, int y, char c);
void drawLine(Canvas *canvas, int x1, int y1, int x2, int y2, char c);
void drawCircle(Canvas *canvas, int cx, int cy, int radius, char c);
void drawRectangle(Canvas *canvas, int x1, int y1, int x2, int y2, char c);
void drawTriangle(Canvas *canvas, int x1, int y1, int x2, int y2, int x3, int y3, char c);
void renderObject(Canvas *canvas, Shape *shape);
int addObject(Canvas *canvas, ShapeType type, int x1, int y1, int x2, int y2, char fillChar);
void deleteObject(Canvas *canvas, int id);
void modifyObject(Canvas *canvas, int id, int x1, int y1, int x2, int y2);
void displayCanvas(Canvas *canvas);
void listObjects(Canvas *canvas);
void redrawCanvas(Canvas *canvas);

// Initialize canvas
Canvas* initCanvas() {
    Canvas *canvas = (Canvas*)malloc(sizeof(Canvas));
    canvas->objectCount = 0;
    canvas->nextId = 1;
    clearCanvas(canvas);
    return canvas;
}

// Clear canvas with spaces
void clearCanvas(Canvas *canvas) {
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            canvas->canvas[y][x] = ' ';
        }
    }
}

// Draw a single pixel on canvas
void drawPixel(Canvas *canvas, int x, int y, char c) {
    if (x >= 0 && x < CANVAS_WIDTH && y >= 0 && y < CANVAS_HEIGHT) {
        canvas->canvas[y][x] = c;
    }
}

// Bresenham's line drawing algorithm
void drawLine(Canvas *canvas, int x1, int y1, int x2, int y2, char c) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int x = x1, y = y1;

    while (1) {
        drawPixel(canvas, x, y, c);
        if (x == x2 && y == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

// Draw circle using Midpoint Circle Algorithm
void drawCircle(Canvas *canvas, int cx, int cy, int radius, char c) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        // Draw 8 symmetric points
        drawPixel(canvas, cx + x, cy + y, c);
        drawPixel(canvas, cx - x, cy + y, c);
        drawPixel(canvas, cx + x, cy - y, c);
        drawPixel(canvas, cx - x, cy - y, c);
        drawPixel(canvas, cx + y, cy + x, c);
        drawPixel(canvas, cx - y, cy + x, c);
        drawPixel(canvas, cx + y, cy - x, c);
        drawPixel(canvas, cx - y, cy - x, c);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// Draw rectangle outline
void drawRectangle(Canvas *canvas, int x1, int y1, int x2, int y2, char c) {
    // Ensure coordinates are ordered correctly
    int minX = (x1 < x2) ? x1 : x2;
    int maxX = (x1 > x2) ? x1 : x2;
    int minY = (y1 < y2) ? y1 : y2;
    int maxY = (y1 > y2) ? y1 : y2;

    // Draw top and bottom edges
    for (int x = minX; x <= maxX; x++) {
        drawPixel(canvas, x, minY, c);
        drawPixel(canvas, x, maxY, c);
    }

    // Draw left and right edges
    for (int y = minY; y <= maxY; y++) {
        drawPixel(canvas, minX, y, c);
        drawPixel(canvas, maxX, y, c);
    }
}

// Draw triangle outline
void drawTriangle(Canvas *canvas, int x1, int y1, int x2, int y2, int x3, int y3, char c) {
    // Draw three lines connecting the vertices
    drawLine(canvas, x1, y1, x2, y2, c);
    drawLine(canvas, x2, y2, x3, y3, c);
    drawLine(canvas, x3, y3, x1, y1, c);
}

// Render a shape object on the canvas
void renderObject(Canvas *canvas, Shape *shape) {
    switch (shape->type) {
        case CIRCLE: {
            int radius = abs(shape->x2 - shape->x1);
            drawCircle(canvas, shape->x1, shape->y1, radius, shape->fillChar);
            break;
        }
        case RECTANGLE:
            drawRectangle(canvas, shape->x1, shape->y1, shape->x2, shape->y2, shape->fillChar);
            break;
        case LINE:
            drawLine(canvas, shape->x1, shape->y1, shape->x2, shape->y2, shape->fillChar);
            break;
        case TRIANGLE:
            drawTriangle(canvas, shape->x1, shape->y1, shape->x2, shape->y2, 
                        (shape->x1 + shape->x2) / 2, shape->y2, shape->fillChar);
            break;
    }
}

// Add object to the scene
int addObject(Canvas *canvas, ShapeType type, int x1, int y1, int x2, int y2, char fillChar) {
    if (canvas->objectCount >= MAX_OBJECTS) {
        printf("Error: Maximum objects reached!\n");
        return -1;
    }

    Shape *newShape = &canvas->objects[canvas->objectCount];
    newShape->id = canvas->nextId++;
    newShape->type = type;
    newShape->x1 = x1;
    newShape->y1 = y1;
    newShape->x2 = x2;
    newShape->y2 = y2;
    newShape->fillChar = fillChar;

    canvas->objectCount++;
    return newShape->id;
}

// Delete object by ID
void deleteObject(Canvas *canvas, int id) {
    int found = -1;
    for (int i = 0; i < canvas->objectCount; i++) {
        if (canvas->objects[i].id == id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("Error: Object with ID %d not found!\n", id);
        return;
    }

    // Shift objects down
    for (int i = found; i < canvas->objectCount - 1; i++) {
        canvas->objects[i] = canvas->objects[i + 1];
    }
    canvas->objectCount--;
    printf("Object %d deleted successfully.\n", id);
}

// Modify object coordinates
void modifyObject(Canvas *canvas, int id, int x1, int y1, int x2, int y2) {
    for (int i = 0; i < canvas->objectCount; i++) {
        if (canvas->objects[i].id == id) {
            canvas->objects[i].x1 = x1;
            canvas->objects[i].y1 = y1;
            canvas->objects[i].x2 = x2;
            canvas->objects[i].y2 = y2;
            printf("Object %d modified successfully.\n", id);
            return;
        }
    }
    printf("Error: Object with ID %d not found!\n", id);
}

// Redraw all objects on the canvas
void redrawCanvas(Canvas *canvas) {
    clearCanvas(canvas);
    for (int i = 0; i < canvas->objectCount; i++) {
        renderObject(canvas, &canvas->objects[i]);
    }
}

// Display the canvas
void displayCanvas(Canvas *canvas) {
    printf("\n");
    // Top border
    printf("+");
    for (int x = 0; x < CANVAS_WIDTH; x++) printf("-");
    printf("+\n");

    // Canvas content
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        printf("|");
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            printf("%c", canvas->canvas[y][x]);
        }
        printf("|\n");
    }

    // Bottom border
    printf("+");
    for (int x = 0; x < CANVAS_WIDTH; x++) printf("-");
    printf("+\n\n");
}

// List all objects in the scene
void listObjects(Canvas *canvas) {
    if (canvas->objectCount == 0) {
        printf("\nNo objects in the scene.\n");
        return;
    }

    printf("\n=== Objects in Scene ===\n");
    printf("ID | Type      | X1,Y1    | X2,Y2    | Fill\n");
    printf("---+-----------+----------+----------+-----\n");

    for (int i = 0; i < canvas->objectCount; i++) {
        Shape *s = &canvas->objects[i];
        const char *typeStr;
        switch (s->type) {
            case CIRCLE: typeStr = "Circle"; break;
            case RECTANGLE: typeStr = "Rectangle"; break;
            case LINE: typeStr = "Line"; break;
            case TRIANGLE: typeStr = "Triangle"; break;
            default: typeStr = "Unknown"; break;
        }
        printf("%2d | %-9s | (%2d,%2d)  | (%2d,%2d)  | '%c'\n",
               s->id, typeStr, s->x1, s->y1, s->x2, s->y2, s->fillChar);
    }
    printf("\n");
}

// Interactive menu
void showMenu() {
    printf("\n=== 2D Graphics Editor ===\n");
    printf("1. Add Circle\n");
    printf("2. Add Rectangle\n");
    printf("3. Add Line\n");
    printf("4. Add Triangle\n");
    printf("5. Delete Object\n");
    printf("6. Modify Object\n");
    printf("7. List Objects\n");
    printf("8. Display Canvas\n");
    printf("9. Clear Canvas\n");
    printf("0. Exit\n");
    printf("Choose an option: ");
}

int main() {
    Canvas *canvas = initCanvas();
    int choice;

    printf("Welcome to the 2D Graphics Editor!\n");
    printf("Canvas size: %d x %d\n", CANVAS_WIDTH, CANVAS_HEIGHT);

    while (1) {
        showMenu();
        scanf("%d", &choice);
        getchar(); // Consume newline

        switch (choice) {
            case 1: {
                // Add Circle
                int cx, cy, radius;
                printf("Enter center X, Y and radius: ");
                scanf("%d %d %d", &cx, &cy, &radius);
                getchar();
                int id = addObject(canvas, CIRCLE, cx, cy, cx + radius, cy, '*');
                printf("Circle added with ID %d\n", id);
                redrawCanvas(canvas);
                break;
            }
            case 2: {
                // Add Rectangle
                int x1, y1, x2, y2;
                printf("Enter X1, Y1, X2, Y2: ");
                scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
                getchar();
                int id = addObject(canvas, RECTANGLE, x1, y1, x2, y2, '*');
                printf("Rectangle added with ID %d\n", id);
                redrawCanvas(canvas);
                break;
            }
            case 3: {
                // Add Line
                int x1, y1, x2, y2;
                printf("Enter X1, Y1, X2, Y2: ");
                scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
                getchar();
                int id = addObject(canvas, LINE, x1, y1, x2, y2, '*');
                printf("Line added with ID %d\n", id);
                redrawCanvas(canvas);
                break;
            }
            case 4: {
                // Add Triangle
                int x1, y1, x2, y2;
                printf("Enter base X1, Y1, X2, Y2: ");
                scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
                getchar();
                int id = addObject(canvas, TRIANGLE, x1, y1, x2, y2, '*');
                printf("Triangle added with ID %d\n", id);
                redrawCanvas(canvas);
                break;
            }
            case 5: {
                // Delete Object
                int id;
                printf("Enter object ID to delete: ");
                scanf("%d", &id);
                getchar();
                deleteObject(canvas, id);
                redrawCanvas(canvas);
                break;
            }
            case 6: {
                // Modify Object
                int id, x1, y1, x2, y2;
                printf("Enter object ID and new coordinates (X1 Y1 X2 Y2): ");
                scanf("%d %d %d %d %d", &id, &x1, &y1, &x2, &y2);
                getchar();
                modifyObject(canvas, id, x1, y1, x2, y2);
                redrawCanvas(canvas);
                break;
            }
            case 7: {
                // List Objects
                listObjects(canvas);
                break;
            }
            case 8: {
                // Display Canvas
                displayCanvas(canvas);
                break;
            }
            case 9: {
                // Clear Canvas
                canvas->objectCount = 0;
                canvas->nextId = 1;
                clearCanvas(canvas);
                printf("Canvas cleared.\n");
                break;
            }
            case 0: {
                printf("Thank you for using the 2D Graphics Editor!\n");
                free(canvas);
                return 0;
            }
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}